#!/usr/bin/perl

# Guru module: Translate single words using the dict sources
# IDP sources can be converted to get a larger dictionary base
# Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
# Published under GNU GPL conditions

use strict;

# Commands:
# guru what is foo in de				-> translate from en to de
# guru what is bar in fuzzy english		-> translate from de to en, use fuzzy match

my $dictdir = "/usr/share/trans";

###################################################################################

sub i18n{my $foo = shift(@_);return $foo;}

my $inputline = <STDIN>;
chomp($inputline);
my @input = split(/\ /, $inputline);

my $letthrough;
$letthrough = 0;
if(($input[1] eq "what") && ($input[2] eq "is")){
	if($input[3] ne ""){
		if(($input[4] eq "in") && ($input[5] ne "")){
			$letthrough = 1;
		}else{
			print i18n("That\'s a word. Come on, I don\'t know your native language yet.");
		}
	}else{
		print i18n("What is what dude?");
	}
}
if(!$letthrough){
	close(STDOUT);
	sleep(1);
	exit(0);
}

my ($fuzzy, $LANG, $WORD);
$fuzzy = 0;
if($input[5] eq "fuzzy"){
	$LANG = $input[6];
	$fuzzy = 1;
}else{
	$LANG = $input[5];
}
$WORD = $input[3];

# make configurable!
my $FROM = "en";

if($LANG eq "german"){$LANG = "de";}
if($LANG eq "italian"){$LANG = "it";}
if($LANG eq "spanish"){$LANG = "es";}
if($LANG eq "portuguese"){$LANG = "pt";}
if($LANG eq "english"){$LANG = "en";}

if(($LANG ne "de") && ($LANG ne "en") && ($LANG ne "it") && ($LANG ne "pt") && ($LANG ne "es")){
	print i18n("I\'m so uneducated I only know english, german, spanish, portuguese, italian.");
	close(STDOUT);
	sleep(1);
	exit(0);
}

my $reverse = 1;
if($LANG eq "en"){
	$FROM = "de";
	$reverse = 0;
}
if($LANG eq "de"){
}
if($LANG eq "pt"){
	$reverse = 0;
}
if($LANG eq "it"){
	$reverse = 0;
}
if($LANG eq "es"){
	$reverse = 0;
}

my $file;
if($reverse){
	$file = "$LANG-$FROM";
}else{
	$file = "$FROM-$LANG";
}

my (@ar, $exp, $answer, $num);
open(FILE, "$dictdir/$file");
while(<FILE>){
	chomp;
	s/\ \{([mfn]|pl)\}//g;
	@ar = split(/\ ::\ /);
	$exp = $ar[$reverse];
	if(($exp =~ /^$WORD$/i) || (($fuzzy) && ($exp =~ /$WORD/i) && ($num < 5))){
		if($answer){
			$answer .= ", ";
			$num++;
		}
		$answer .= $ar[!$reverse];
	}
}
close(FILE);
if($answer){
	print "$answer\n";
}else{
	print i18n("Sorry, I don't know what a"), " $WORD ", i18n("is.");
}
close(STDOUT);
sleep(1);

