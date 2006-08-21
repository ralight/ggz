#!/usr/bin/perl
#
# AI player for the Muehle game
# Copyright (C) 2002? - 2006 Josef Spillner <josef@ggzgamingzone.org>

# Include these perl modules
use Socket;
use IO::Handle;

# Find out where GGZ is installed
$gamedir = `ggz-config -g`;
$datadir = `ggz-config -d`;
if (!$gamedir || !$datadir){
	print "Error: ggz-config invocation failed.\n";
	exit 1;
}
chomp $gamedir;
chomp $datadir;

# Define variables
socketpair(CHILD, PARENT, AF_UNIX, SOCK_STREAM, PF_UNSPEC);
PARENT->autoflush(1);
CHILD->autoflush(1);
$empty = 1;
$white = 2;
$black = 3;

# Main loop (client-server interaction)
$ret = fork();
if($ret < 0){
	exit 1;
}elsif(!$ret){
	close(CHILD);
	close(STDIN);
	open(STDIN, ">&PARENT");
	open(STDOUT, ">&PARENT");
	system("$gamedir/muehle --ggz");
	close(PARENT);
	exit 0;
}else{
	close(PARENT);
	createboard();
	$greeting = <CHILD>;
	chomp $greeting;
	print "Greeter: $greeting\n";
	print CHILD "white.\n";
	while(<CHILD>){
		chomp;
		process($_);
	}
	close(CHILD);
}

# Create board
sub createboard{
	my $i = 0;

	open(LEVEL, "$datadir/muehle/classic");
	while(<LEVEL>){
		next if /^\#/ || /^\/\//;
		next if $_ eq "\n";
		chomp;
		$_ =~ s/\(//g;
		$_ =~ s/\)//g;
		$_ =~ s/\ //g;
		@quad = split(/\,/);
		$quadlist[$i][0] = $quad[0];
		$quadlist[$i][1] = $quad[1];
		$quadlist[$i][2] = $quad[2];
		$quadlist[$i][3] = $quad[3];
		$i++;
	}
	close(LEVEL);

	for $j(1..9){
		for $i(1..9){
			$board[$i][$j] = $empty;
		}
	}
}

# Dump the current board
sub dumpboard{
	print "\n";
	for $j(1..9){
		for $i(1..9){
			if(exist($i * 10, $j * 10)){
				print $board[$i][$j];
			}else{
				print " ";
			}
		}
		print "\n";
	}
	print "\n";
}

# AI routines
sub ai{
	my $xx = shift(@_);
	my $yy = shift(@_);
	my $i = 0;
	my $xrows = 0;
	my $yrows = 0;

	# Detecting muehles
	while($quadlist[$i]){
		$x = $quadlist[$i][0];
		$y = $quadlist[$i][1];
		$x2 = $quadlist[$i][2];
		$y2 = $quadlist[$i][3];
		if($x == $xx){
			$xrows++;
		}
		if($y == $yy){
			$yrows++;
		}
		$i++;
	}
	if(($xrows == 3) || ($yrows == 3)){
		$x = 0;
		$y = 0;
		for $j(1..9){
			for $i(1..9){
				if($board[$i][$j] == $white){
					$x = $i;
					$y = $j;
				}
			}
		}
		if($x && $y){
			print "Debug: CHILD TAKE\n";
			print CHILD "[$x, $y].\n";
			return;
		}
	}

	# Normal stone setting
	for $j(1..9){
		for $i(1..9){
			if(exist($i * 10, $j * 10) && ($board[$i][$j] == $empty)){
				$board[$i][$j] = $black;
				$x = $i * 10;
				$y = $j * 10;
				print CHILD "[$x,$y].\n";
				print "Debug: CHILD [$x,$y].\n";
				return;
			}
		}
	}
}

# Evaluate a move
sub process{
	my $move = shift(@_);
	$move =~ s/\[//g;
	$move =~ s/\]//g;
	$move =~ s/\.//g;
	print "Move: $move\n";
	my $size = split(/,/, $move);
	my @ar = split(/,/, $move);
	my $x = $ar[0] / 10;
	my $y = $ar[1] / 10;

	if($size == 2){
		print "* Set stone to $x/$y\n";
		$board[$x][$y] = $white;
	}elsif($size == 4){
		$x2 = $ar[2] / 10;
		$y2 = $ar[3] / 10;
		print "* Move from $x/$y to $x2/$y2\n";
		$board[$x][$y] = 0;
		$board[$x][$y] = $white;
	}

	dumpboard();

	ai($x, $y);
}

# ...
sub exist{
	my $xx = shift(@_);
	my $yy = shift(@_);
	my $i = 0;

	while($quadlist[$i]){
		$x = $quadlist[$i][0];
		$y = $quadlist[$i][1];
		$x2 = $quadlist[$i][2];
		$y2 = $quadlist[$i][3];
		if(($xx == $x) && ($yy == $y)){return 1;}
		if(($xx == $x2) && ($yy == $y2)){return 1;}
		$i++;
	}
	return 0;
}

