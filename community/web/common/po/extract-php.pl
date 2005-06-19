#!/usr/bin/env perl
#
# Extract all __("x") values from *.php files.
# Needs to be called after extract.pl.

use strict;

my $s;
my %msgids;

my $regexp = qr/\_\_\(\"([^\"\)]*)\"\)/;

for my $i(0..$#ARGV){
	open(F, $ARGV[$i]);
	while(<F>){
		my $s = $_;
		my @words = ($s =~ /$regexp/g);
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

