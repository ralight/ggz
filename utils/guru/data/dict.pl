#!/usr/bin/perl

# guru what is foo in german
# guru what is bar in fuzzy english

$letthrough = 0;
if(($ARGV[1] eq "what") && ($ARGV[2] eq "is")){
	if($ARGV[3] ne ""){
		if(($ARGV[4] eq "in") && ($ARGV[5] ne "")){
			$letthrough = 1;
		}else{
			print "That\'s a word. Come on, I don\'t know your native language yet.\n";
		}
	}else{
		print "What is what dude?\n";
	}
}
if(!$letthrough){
	exit(0);
}

$fuzzy = 0;
if($ARGV[5] eq "fuzzy"){
	$LANG = $ARGV[6];
	$fuzzy = 1;
}else{
	$LANG = $ARGV[5];
}
$WORD = $ARGV[3];

# make configurable!
$FROM = "en";

if($LANG eq "german"){$LANG = "de";}
if($LANG eq "italian"){$LANG = "it";}
if($LANG eq "spanish"){$LANG = "es";}
if($LANG eq "portuguese"){$LANG = "pt";}
if($LANG eq "english"){$LANG = "en";}

if(($LANG ne "de") && ($LANG ne "en") && ($LANG ne "it") && ($LANG ne "pt") && ($LANG ne "es")){
	print "I\'m so uneducated I only know english, german, spanish, portuguese, italian.\n";
	exit(0);
}

$reverse = 1;
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

if($reverse){
	$file = "$LANG-$FROM";
}else{
	$file = "$FROM-$LANG";
}

open(FILE, "/usr/share/trans/$file");
while(<FILE>){
	chomp;
	s/\ \{([mfn]|pl)\}//g;
	@ar = split(/\ ::\ /);
	$exp = $ar[$reverse];
	if((($exp =~ /^$WORD$/i) && ($num < 5)) || (($fuzzy) && ($exp =~ /$WORD/i))){
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
	print "Sorry, I don't know what a $WORD is.\n";
}

