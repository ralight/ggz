#!/usr/bin/perl

$lasttime = 0;

while(!eof(STDIN)){
	$_ = <STDIN>;
	if($_ < $lasttime){
		print("$lasttime");
	}
	$lasttime = $_;
}

print("$_");
