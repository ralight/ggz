#!/usr/bin/env perl
#
# Extract all _(x) values from *.inc(.*) files.

use strict;

my ($s, $s2);
my %msgids;

for my $i(0..$#ARGV){
	open(F, $ARGV[$i]);
	while(<F>){
		my $s = $_;
		my @words = ($s =~ /\_\(([^\)]*)\)/g);
		for $s(@words){
			if(!$msgids{$s}){
				print "msgid \"$s\"\n";
				print "msgstr \"\"\n";
				print "\n";
				$msgids{$s} = 1;
			}
		}
	}
	close(F);
}

