#!/usr/bin/perl

if($ARGV[0] ne "news"){
	exit;
}

sub xml{
	$arg = shift(@_);
	@ar1 = split(/\>/, $arg);
	@ar2 = split(/\</, $ar1[1]);
	$ret = $ar2[0];
	return $ret;
}

$r = rand(100) % 10 + 2;
open(FILE, "../data/slashdot.rdf") or die "File not found!\n";
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

