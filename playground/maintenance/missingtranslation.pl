#!/usr/bin/perl
#
# call: ./missingtranslation.pl in webroot
# content negotiation fix: add --defaults

use strict;

my @files;
my @allfiles;
my %langs;
my %missing;
my $defaults;

if($ARGV[0] eq "--defaults"){
	$defaults = 1;
	$langs{"php"} = 1;
}

sub dirlist{
	my $dir = shift(@_);
	my @paths;

	opendir(DIR, $dir);
	while(my $entry = readdir(DIR)){
		push @paths, "$dir/$entry";
	}
	closedir(DIR);
	foreach my $path(@paths){
		if(-f $path){
			push @allfiles, $path;
		}elsif(!($path =~ /\/*\.+$/)){
			dirlist($path);
		}
	}
}

dirlist(".");

foreach my $file(@allfiles){
	if($file =~ /\.php\./){
		push @files, $file;
		my $lang = $file;
		$lang =~ s/[a-z0-9\.\/\-]+\.php\.//;
		$langs{$lang} = 1;
		#print "add $lang\n";
	}
}

foreach my $file(sort(@files)){
	#print "* $file\n";
	my $lang = $file;
	my $base = $file;
	$lang =~ s/[a-z0-9\.\/\-]+\.php\.//;
	$base =~ s/\.[a-z\-]+$//;
	#print "($base :: $lang)\n";
	$missing{$base}{$lang} = 1;
}

foreach my $base(sort(keys(%missing))){
	if(!$missing{$base}{"php"}){
		my $slang = "";
		if($missing{$base}{"en"}){
			$slang = "en";
		}else{
			foreach my $lang(keys(%langs)){
				if($missing{$base}{$lang}){
					$slang = $lang;
				}
			}
		}
		print "# generating: $base default from $slang\n";
		system("cp $base.$slang $base.php");
	}
}

foreach my $base(sort(keys(%missing))){
	print "# missing: $base ( ";
	foreach my $lang(keys(%langs)){
		if(!$missing{$base}{$lang}){
			print "$lang ";
		}
	}
	print ")\n";
}

