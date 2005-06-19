#!/usr/bin/env perl
#
# Extract all _(x) values from *.inc(.*) files.

use strict;

my ($s, $tag);
my %msgids;

print "msgid \"\"\n";
print "msgstr \"Content-Type: text/plain; charset=UTF-8\\n\"\n";
print "\n";

my $tagexp = qr/[^\_]\_\(([^\)]*\)\([^\)]*)\)/;
my $regexp = qr/[^\_]\_\(([^\)]*)\)/;

for my $i(0..$#ARGV){
	open(F, $ARGV[$i]);
	while(<F>){
		my $s = $_;
		my @taggedwords = ($s =~ /$tagexp/g);
		$s =~ s/$tagexp//;
		my @words = ($s =~ /$regexp/g);
		for $s(@words, @taggedwords){
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

