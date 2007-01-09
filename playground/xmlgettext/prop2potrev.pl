#!/usr/bin/env perl

if($ARGV[0]){$f = $ARGV[0];}else{$f = "messages.properties";}
if($ARGV[1]){$f2 = $ARGV[1];}else{$f2 = "";}

%prop = ();
%trans = ();

open(F, $f) || die "Cannot open $f";
while(<F>){
	chomp;
	$_ =~ s/\r//;
	next if /^#/ || /^$/;

	($id, $original) = split(/=/, $_, 2);
	$original =~ s/\\n/\"\n\"/g;
	$original =~ s/\\ /\ /g;

	push @{$prop{$original}}, $id;
}
close(F);

if($f2){
open(F, $f2) || die "Cannot open $f2";
while(<F>){
	chomp;
	$_ =~ s/\r//;
	next if /^#/ || /^$/;

	($id, $translation) = split(/=/, $_, 2);
	$translation =~ s/\\n/\"\n\"/g;
	$translation =~ s/\\ /\ /g;

	for my $original(keys(%prop)){
		@l = @{$prop{$original}};
		foreach $id2(@l){
			if($id2 eq $id){
				$trans{$original} = $translation;
			}
		}
	}
}
close(F);
}

for my $original(keys(%prop)){
	@l = @{$prop{$original}};
	$translation = $trans{$original};

	print "\n";
	print "#. PROPERTY";
	foreach $id(@l){
		print " $id";
	}
	print "\n";
	print "msgid \"$original\"\n";
	print "msgstr \"$translation\"\n";
}

