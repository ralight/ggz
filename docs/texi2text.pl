#!/usr/bin/perl

use strict;

my($body, $line, $oldline);

open(TEXI, $ARGV[0]) || die;
while(<TEXI>){
	if(/^\@titlepage\n$/){$body = 1;}
	next if not $body;

	$line = "  $_";

	$line =~ s/\@copyright\{\}/\(C\)/g;
	$line =~ s/\@command\{([^\}]+)\}/\[$1\]/g;
	$line =~ s/\@url\{([^\}]+)\}/\<\<$1\>\>/g;
	$line =~ s/\@verbatim//g;
	$line =~ s/\@end\ verbatim\n//g;
	$line =~ s/\@itemize(\ \@bullet)*//g;
	$line =~ s/\@end\ itemize\n//g;
	$line =~ s/\@item\ /\ * /g;
	$line =~ s/\@c\ ([^\n]+)//g;
	$line =~ s/\@setfilename\ ([^\n]+)//g;
	$line =~ s/\@setchapternewpage\ ([^\n]+)//g;
	$line =~ s/\@contents\n//g;
	$line =~ s/\@titlepage//g;
	$line =~ s/\@page//g;
	$line =~ s/\@vskip\ ([^\n]+)//g;
	$line =~ s/\@end\ titlepage//g;
	$line =~ s/\@node\ ([^\n]+)//g;
	$line =~ s/\@bye\n//g;
	$line =~ s/\ \ \@appendix/Appendix\:/g;
	$line =~ s/\ \@section/Section\:/g;
	$line =~ s/\@subsection/Section\:/g;
	$line =~ s/\ \ \@chapter/Chapter\:/g;
	$line =~ s/\@ifinfo//g;
	$line =~ s/\@end\ ifinfo//g;
	$line =~ s/\ \ \@menu/Menu\:/g;
	$line =~ s/\@end\ menu\n//g;
	$line =~ s/\@title\ ([^\n]+)\n/\*\*\*\*\*\ $1 \*\*\*\*\*\n/g;
	$line =~ s/\@subtitle\ ([^\n]+)\n/\*\*\*\ $1 \*\*\*\n\n/g;
	$line =~ s/\@author/Author\:/g;

	$line =~ s/^\ \ $//g;

	if($oldline ne $line){print $line;}
	$oldline = $line;
}
close(TEXI);

