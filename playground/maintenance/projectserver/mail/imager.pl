#!/usr/bin/perl

use GD::Graph::area;
use strict;

my $file = $ARGV[0];
$file or die;

my $list = $file;
$list =~ s/([^\.]*).*/$1/;

my @monthnames = (
	"January", "February", "March", "April", "May", "June", "July", "August",
	"September", "October", "November", "December"
);

my %years = ();
my %monthyears = ();
my @numbers = ();
my @months = ();
my @tmpnumbers = ();
my @tmpmonths = ();
my ($monthyear, $number, $year, $month);
open(F, $file);
while(<F>){
	chomp;
	$monthyear = $_;
	($year, $month) = split(/\-/);
	$_ = <F>;
	chomp;
	$number = $_;
	$years{$year} = 1;
	$monthyears{$monthyear} = $number;
}
close(F);

my $i = 0;
my $last = 0;
for $year(sort(keys(%years))){
	#print "*** $year\n";
	for $month(@monthnames){
		$monthyear = "$year-$month";
		if($monthyears{$monthyear}){
			$number = $monthyears{$monthyear};
		}else{
			$number = 0;
		}
		#print "### $month: $number\n";
		if($number > 0){
			$last = $i;
		}
		push @tmpnumbers, $number;
		push @tmpmonths, "$month $year";
		$i += 1;
	}
}

for $i(0..$last){
	$month = @tmpmonths[$i];
	$number = @tmpnumbers[$i];
	print ">>> $month: $number\n";
	push @numbers, $number;
	push @months, $month;
}

my @data = ();
push @data, [@months];
push @data, [@numbers];

my $mygraph = GD::Graph::area->new(500, 300);
$mygraph->set(
	x_label => "Month",
	y_label => "Postings",
	title => "Activity of $list mailing list",
	x_labels_vertical => 1
) or warn $mygraph->error;

my $myimage = $mygraph->plot(\@data) or die $mygraph->error;

#print $myimage->png;
open(F, ">out.png");
print F $myimage->png;
close(F);

