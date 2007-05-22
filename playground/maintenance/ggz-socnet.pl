#!/usr/bin/perl
#
# Creates a .dot file representing a 'social network' of GGZ game matches
# Copyright (C) 2007 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions
#
# Run: ggz-socnet.pl > foo.dot; dot -T -o foo.png foo.dot

my $excludebots = 1;

## End of user settings

use strict;
use DBI;
use Config::IniFiles;

## Find out where GGZ is installed

my $confdir = `ggz-config -c`;
if(!$confdir){
	print "Error: ggz-config invocation failed.\n";
	exit 1;
}
chomp $confdir;

## Load configuration files

my $conffile = "$confdir/ggzd/ggzd.conf";

my $cfg = new Config::IniFiles(-file => $conffile);
if(!$cfg){
	print "Error: ggzd configuration file '$conffile' not found.\n";
	exit 1;
}

## connection parameters for GGZ

my $ggz_type = "Pg";
my $ggz_host = $cfg->val("General", "DatabaseHost");
my $ggz_name = $cfg->val("General", "DatabaseName");
my $ggz_user = $cfg->val("General", "DatabaseUsername");
my $ggz_pass = $cfg->val("General", "DatabasePassword");

### end of configuration

my $ggz_conn = DBI->connect("DBI:$ggz_type:host=$ggz_host;dbname=$ggz_name;user=$ggz_user;password=$ggz_pass");
$ggz_conn || die;

my ($match, $handle, $playertype);
my ($res, $res2);
my (%relation, %allplayers);

$res = $ggz_conn->prepare("SELECT DISTINCT `match` FROM `matchplayers`");
$res->execute();
$res->bind_columns(undef, \$match);

while($res->fetch()){
	$res2 = $ggz_conn->prepare("SELECT `handle`, `playertype` FROM `matchplayers` WHERE `match` = $match");
	$res2->execute();
	$res2->bind_columns(undef, \$handle, \$playertype);
	my @players = ();
	while($res2->fetch()){
		if($excludebots){
			if(($playertype eq "bot") || ($handle =~ /\d\|AI/)){
				# FIXME: \d|AI is backward compat only!
				next;
			}
			push @players, $handle;
		}
	}
	$res2->finish();

	#print "* $match: ", @players, "\n";
	foreach my $p1(@players){
		$allplayers{$p1} = 1;
		# FIXME: handle playertype (guest, bot, registered)
		foreach my $p2(@players){
			if($p1 ne $p2){
				#print " - $p1+$p2\n";
				my $forward = "\"$p1\" -- \"$p2\"";
				my $reverse = "\"$p2\" -- \"$p1\"";
				if(!$relation{$reverse}){
					$relation{$forward} += 1;
				}
			}
		}
	}
}

$res->finish();

$ggz_conn->disconnect();

print "strict graph \"GGZ player relationships\" {\n";

foreach my $p(keys(%allplayers)){
	print "\"$p\" [style=filled, fillcolor=lightyellow, shape=box];\n";
}

foreach my $rel(keys(%relation)){
	my $weight = $relation{$rel};
	#print "$rel: $weight\n";
	print "$rel [weight=$weight];\n";
}

print "}\n";

