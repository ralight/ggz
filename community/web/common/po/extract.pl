#!/usr/bin/env perl
#
# Extract all _(x) values from *.inc(.*) files.
# Can now cope with _(x (y)) as well.

use strict;

my ($s, $tag);
my %msgids;

print "# Pseudo-entry to force the correct encoding\n";
print "msgid \"\"\n";
print "msgstr \"Content-Type: text/plain; charset=UTF-8\\n\"\n";
print "\n";

my $tagexp = qr/(?:[^\_]|^)\_\((?:[^\)]*)\)\((?:[^\)]*)\)/;
my $regexp = qr/(?:[^\_]|^)\_\((?:[^\)]*)\)/;
my $regexpparens = qr/(?:[^\_]|^)\_\((?:[^\(\)]*)\((?:[^\)]*)\)(?:[^\)]*)\)/;
my $finalexp = qr/[^\_]?\_\(((?:[^\(\)]*)\(?(?:[^\)]*)\)?(?:[^\(\)]*))\)(?:\(([^\)]*)\))?/;

for my $i(0..$#ARGV){
	open(F, $ARGV[$i]);
	my $line = 1;
	while(<F>){
		my $s = $_;
		my @taggedwords = ($s =~ /$tagexp/g);
		$s =~ s/$tagexp//g;
		my @wordsparens = ($s =~ /$regexpparens/g);
		$s =~ s/$regexpparens//g;
		my @words = ($s =~ /$regexp/g);
		for $s(@wordsparens, @words, @taggedwords){
			$s =~ s/^[^\_]//;
			if(!$msgids{$s}){
				my ($word, $tag) = ($s =~ /$finalexp/);
				print "# Template: $ARGV[$i]:$line\n";
				if($tag){
					print "# word: $word, tag: $tag\n";
					$word = "$word($tag)";
				}
				print "msgid \"$word\"\n";
				print "msgstr \"\"\n";
				print "\n";
				$msgids{$s} = 1;
			}
		}
		$line++;
	}
	close(F);
}

