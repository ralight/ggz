#!/usr/bin/perl
# Colorsniff
# Utily to track GGZ data exchange
# Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net

use Term::ANSIColor;

if($ARGV[0] eq "--noping"){
	$noping = 1;
}

# QUIT, PIPE, INT, TERM, huh???
$SIG{'INT'}='aurevoir';

sub bienvenue{
	$color1 = color("green");
	$color2 = color("cyan");

	print color("red");
	print "GGZ protocol sniffer ready.\n";
	print color("reset");
}

sub aurevoir{
	print color("red");
	print "Bye, bye...\n";
	print color("reset");
	exit;
}

bienvenue;

#open(F, "sniff.log");
while(<STDIN>){
	chomp;
	next if((/^#/) || (/^interface/));

	if(/^T\ /){
		@addr = split;
		$peer1 = $addr[1];
		$peer2 = $addr[3];
		#print "($peer1) -> ($peer2)\n";
		if(!$xpeer1){
			$xpeer1 = $peer1;
			$xpeer2 = $peer2;
		}
		if($peer1 eq $xpeer1){
			$color = $color1;
		}else{
			$color = $color2;
		}
	}else{
		s/\>\.\./\>/g;
		s/\>\./\>/g;
		next if(($noping) && ((/\<PING\/\>/) || (/\<PONG\/\>/)));
		print $color;
		print "$_\n";
	}
}
#close(F);

aurevoir;

