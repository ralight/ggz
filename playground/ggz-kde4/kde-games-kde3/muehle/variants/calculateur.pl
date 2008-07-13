#!/usr/bin/perl
# Calculates you some nice round boards...

$angle = $ARGV[0];
if(!$angle){
	die "Call: calculateur.pl <angle_between_points>\n";
}

if($angle <= 0 || $angle >= 360){
	die "0 < angle < 360!\n";
}

$xpi = 3.14 / 180;
$width = 45;
$height = 45;

$step = 360 / $angle;
$x = $width + 5;
$y = 2 * $height + 5;
$w5 = $width + 5;
$h5 = $height + 5;
for $i(1..$step){
	$xnew = int(sin($i * $angle * $xpi) * $width + $width) + 5;
	$ynew = int(cos($i * $angle * $xpi) * $height + $height) + 5;
	@circle = (@circle, "($x, $y), ($xnew, $ynew)");
	@diagonals = (@diagonals, "($x, $y), ($w5, $h5)");
	$x = $xnew;
	$y = $ynew;
}

print "// circle points\n";
foreach $s(@circle){
	print "$s\n";
}
print "// diagonals\n";
foreach $s(@diagonals){
	print "$s\n";
}

