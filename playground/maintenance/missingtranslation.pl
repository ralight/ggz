#!/usr/bin/perl
#
# call: find . | ./missingtranslation.pl

use strict;

my @files;
my %langs;
my %missing;

while(<STDIN>){
	chomp;
	if(/\.php\./){
		push @files, $_;
		my $lang = $_;
		$lang =~ s/[a-z0-9\.\/]+\.php\.//;
		$langs{$lang} = 1;
		#print "add $lang\n";
	}
}

foreach my $file(sort(@files)){
	#print "* $file\n";
	my $lang = $file;
	my $base = $file;
	$lang =~ s/[a-z0-9\.\/]+\.php\.//;
	$base =~ s/\.[a-z]+$//;
	#print "($base :: $lang)\n";
	$missing{$base}{$lang} = 1;
}

foreach my $base(keys(%missing)){
	foreach my $lang(keys(%langs)){
		if(!$missing{$base}{$lang}){
			print "# missing: $lang for $base\n";
		}
	}
}

