#!/usr/bin/perl

$foo = <STDIN>;
chomp $foo;
$bar = "Uh-oh, someone from userland said $foo *g*";
print $bar;

