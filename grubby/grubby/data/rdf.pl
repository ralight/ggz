#!/usr/bin/perl

# Guru module: Dynamically download news rdf files and display one random
# headline. The rdf files are stored in $gurudir/cache.
# Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
# Published under GNU GPL conditions

use strict;

# Commands:
# guru news					-> list all news sources
# guru news slashdot		-> show random headline from slashdot

my $gurudir = $ENV{HOME} . "/.ggz/grubby";

#########################################################################

use Net::Telnet;

sub i18n{my $foo = shift(@_);return "$foo\n";}

my $inputline = <STDIN>;
chomp($inputline);
my @input = split(/\ /, $inputline);

if($input[1] ne "news"){
	exit();
}
if($input[2] eq ""){
	print i18n("From what news source my friend?");
	print i18n("Available are: slashdot, register, happypenguin, heise, worldforge");
	close(STDOUT);
	sleep(1);
	exit();
}

sub xml{
	my $arg = shift(@_);
	my @ar1 = split(/\>/, $arg);
	my @ar2 = split(/\</, $ar1[1]);
	my $ret = $ar2[0];
	return $ret;
}

my $downloaded;

sub download
{
	my $host = shift(@_);
	my $document = shift(@_);

	my $conn = new Net::Telnet(Host => $host, Port => 80) or die "Sorry, cannot fetch news";
	$conn->print("GET $document HTTP/1.0");
	$conn->print("Host: $host\n");
	my ($data, $tmp);
	while($tmp = $conn->get(Timeout => 10)){
		$data .= $tmp;
	}
	mkdir("$gurudir/cache", 0755);
	open(FILE, ">$gurudir/cache/$input[2].rdf");
	print FILE "$data\n";
	close(FILE);
	$downloaded = 1;
}

$downloaded = 0;
if($input[2] eq "slashdot"){download("slashdot.org", "/slashdot.rdf");}
if($input[2] eq "register"){download("theregister.co.uk", "/tonys/slashdot.rdf");}
if($input[2] eq "happypenguin"){download("happypenguin.org", "/html/news.rdf");}
if($input[2] eq "heise"){download("heise.de", "/newsticker/heise.rdf");}
if($input[2] eq "worldforge"){download("worldforge.org", "/worldforgenews.rdf");}

if(!$downloaded){
	print i18n("News source not available.");
	close(STDOUT);
	sleep(1);
	exit();
}

my $r = rand(100) % 10 + 2;
my $ret = open(FILE, "$gurudir/cache/$input[2].rdf");
if(!$ret){
	print "Sorry, no more news on $input[2].";
	close(STDOUT);
	sleep(1);
	exit();
}

my ($title, $link, $news);
while(<FILE>){
	if($_ =~ /\<title\>/){
		$title = xml($_);
	}
	if($_ =~ /\<link\>/){
		$link = xml($_);
	}
	if(($title) && ($link)){
		if($news == $r){
			print "NEWS! $title ($link)";
			close(STDOUT);
			sleep(1);
		}
		$title = "";
		$link = "";
		$news++;
	}
}
close(FILE);

