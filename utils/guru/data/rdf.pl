#!/usr/bin/perl

use Net::Telnet;

if($ARGV[1] ne "news"){
	exit;
}
if($ARGV[2] eq ""){
	print "From what news source my friend?\n";
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

	$conn = new Net::Telnet(Host => $host, Port => 80);
	$conn->print("GET $document\n");
	$data = $conn->get(Timeout => 5);
	open(FILE, ">$ARGV[2].rdf");
	print FILE "$data\n";
	close(FILE);
	$downloaded = 1;
}

$downloaded = 0;
if($ARGV[2] eq "slashdot"){download("slashdot.org", "/slashdot.rdf");}
if($ARGV[2] eq "register"){download("theregister.co.uk", "/tonys/slashdot.rdf");}
if($ARGV[2] eq "happypenguin"){download("happypenguin.org", "/html/news.rdf");}
if($ARGV[2] eq "heise"){download("heise.de", "/newsticker/heise.rdf");}

if(!$downloaded){
	print "News source not available.\n";
	exit();
}

$r = rand(100) % 10 + 2;
$ret = open(FILE, $ARGV[2] . ".rdf");
if(!$ret){
	print "Sorry, no more news on $ARGV[2].\n";
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
		}
		$title = "";
		$link = "";
		$news++;
	}
}
close(FILE);

