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
$x = $width;
$y = 2 * $height;
for $i(1..$step){
	$xnew = int(sin($i * $angle * $xpi) * $width + $width);
	$ynew = int(cos($i * $angle * $xpi) * $height + $height);
	@circle = (@circle, "($x, $y), ($xnew, $ynew)");
	@diagonals = (@diagonals, "($x, $y), ($width, $height)");
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

