#!/usr/bin/perl

# Guru module: Dynamically download news rdf files and display one random
# headline. The rdf files are stored in $gurudir/cache.
# Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
# Published under GNU GPL conditions

# Commands:
# guru news					-> list all news sources
# guru news slashdot		-> show random headline from slashdot

$gurudir = "/tmp";

#########################################################################

use Net::Telnet;

$inputline = <STDIN>;
chomp($inputline);
@input = split(/\ /, $inputline);

if($input[1] ne "news"){
	exit();
}
if($input[2] eq ""){
	print "From what news source my friend? ";
	print "Available are: slashdot, register, happypenguin, heise\n";
	close(STDOUT);
	sleep(1);
	exit();
}

sub xml{
	my $arg = shift(@_);
	@ar1 = split(/\>/, $arg);
	@ar2 = split(/\</, $ar1[1]);
	$ret = $ar2[0];
	return $ret;
}

sub download
{
	$host = shift(@_);
	$document = shift(@_);

	$conn = new Net::Telnet(Host => $host, Port => 80) or die "Sorry, cannot fetch news\n";
	$conn->print("GET $document\n");
	$data = $conn->get(Timeout => 5);
	mkdir("$gurudir/cache", 0755);
	open(FILE, ">$gurudir/cache/$ARGV[2].rdf");
	print FILE "$data\n";
	close(FILE);
	$downloaded = 1;
}

$downloaded = 0;
if($input[2] eq "slashdot"){download("slashdot.org", "/slashdot.rdf");}
if($input[2] eq "register"){download("theregister.co.uk", "/tonys/slashdot.rdf");}
if($input[2] eq "happypenguin"){download("happypenguin.org", "/html/news.rdf");}
if($input[2] eq "heise"){download("heise.de", "/newsticker/heise.rdf");}

if(!$downloaded){
	print "News source not available.\n";
	close(STDOUT);
	sleep(1);
	exit();
}

$r = rand(100) % 10 + 2;
$ret = open(FILE, "$gurudir/cache/$input[2].rdf");
if(!$ret){
	print "Sorry, no more news on $input[2].\n";
	close(STDOUT);
	sleep(1);
	exit();
}
while(<FILE>){
	if($_ =~ /\<title\>/){
		$title = xml($_);
	}
	if($_ =~ /\<link\>/){
		$link = xml($_);
	}
	if(($title) && ($link)){
		if($news == $r){
			print "NEWS! $title ($link)\n";
			close(STDOUT);
			sleep(1);
		}
		$title = "";
		$link = "";
		$news++;
	}
}
close(FILE);

