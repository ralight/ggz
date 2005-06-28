#!/usr/bin/perl

use strict;

open(X, "page.inc");
my @inc = <X>;
close(X);
open(X, "nointro.inc");
my @inc2 = <X>;
close(X);

my $i = 0;
open(X, "ggzgamingzone.mgp.in");
while(<X>){
	chomp;
	print "$_\n";
	if ($_ eq "%page"){
		print @inc;
		if($i > 0){
			print @inc2;
		}
		$i++;
	}
}
close(X);

