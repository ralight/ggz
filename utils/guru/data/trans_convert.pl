#!/usr/bin/perl
# Converts IDP translation lists to DING-compatible ones

open(IN, "Spanish.txt");
open(OUT, ">es-en");

while(<IN>){
	if(!($_ =~ /\#/)){
		s/\t/\ ::\ /;
		print OUT $_;
	}
}

close(OUT);
close(IN);

