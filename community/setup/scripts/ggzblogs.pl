#!/usr/bin/perl
#
# Planet blog aggregation configuration from GGZ user database
# Copyright (C) 2004 - 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

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

my ($name, $blogfeed);

my $res = $ggz_conn->prepare("SELECT handle, blogfeed FROM userinfo WHERE blogfeed IS NOT NULL AND blogfeed <> ''");
$res->execute();
$res->bind_columns(undef, \$name, \$blogfeed);

while($res->fetch()){
	print "[$blogfeed]\n";
	print "name = $name\n";
	print "\n";
}

$ggz_conn->disconnect();


