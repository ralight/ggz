#!/usr/bin/perl

$GURU = "$ENV{HOME}/.ggz";
$GURUDIR = "/usr/local/share";

mkdir $GURU, 0700 || die "Cannot start setup! Your system is severely broken.";
$GURU = "$GURU/guru";
mkdir $GURU, 0700;

open(FILE, ">$GURU/modexec.rc");
print FILE "[programs]\n";
print FILE "programs = $GURUDIR/data/meeting.rb\n";
close(FILE);

