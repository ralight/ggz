#!/usr/bin/perl

#$kio = "ggz";
#$kde_htmldir = "/usr/local/kde2.2/share/doc/HTML";
$kde_htmldir = $ARGV[0];
$kde_lang = "en";
$appname = "kioslave";
$kio = $ARGV[1];

if((!$kio) || (!$kde_htmldir)){
	print "Error: Missing arguments!\n";
	exit;
}

$docbook = "$kde_htmldir/$kde_lang/$appname/index.docbook";
open(FILE, $docbook) or die "Cannot open $docbook for reading!\n";
@book = <FILE>;
close(FILE);
$done = 0;
foreach $l(@book){
	next if($l =~ /kio\-$kio/);
	if((!$done) && ($l =~ /kio\-/)){
		$z = "<!ENTITY kio-$kio SYSTEM \"$kio.docbook\">\n";
		@book2 = (@book2, $z);
		$done = 1;
	}
	if(($done == 1) && ($l =~ /\&kio\-/)){
		$z = "&kio-$kio;\n";
		@book2 = (@book2, $z);
		$done = 2;
	}
	@book2 = (@book2, $l);
}

open(FILE, ">$docbook") or die "Cannot open $docbook for writing!\n";
print FILE @book2;
close(FILE);

# /usr/local/kde/bin/meinproc --check --cache index.cache.bz2 ./index.docbook

