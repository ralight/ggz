#!/usr/bin/perl
#
# GGZ Gaming Zone Meta Server Configuration
# Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
# Published under GNU GPL conditions

use Net::Telnet;

print "GGZ Gaming Zone Meta Server Configuration\n";
print "=========================================\n\n";

do {
	print "Please enter the URI of the host you want to edit, e.g. ggz://jzaun.com:15689.\n";
	$uri = <STDIN>;
	chomp($uri);
	($proto, $host, $port) = splituri($uri);
} while !$proto || !$host || !$port;

print "DEBUG: $proto, $host, $port\n";

print "Please supply your user name on $uri:\n";
$username = <STDIN>;
chomp($username);
print "Please supply your password on $uri:\n";
$password = <STDIN>;
chomp($password);

do {
	print "\n";
	print "Meta Server Configuration Menu\n";
	print "------------------------------\n";
	#print "1 - View list of GGZ servers\n";
	print "2 - Add GGZ server\n";
	#print "3 - Remove GGZ server\n";
	#print "4 - View list of peers\n";
	#print "5 - Add peer\n";
	#print "6 - Remove peer\n";
	print "7 - Query in URI format\n";
	print "8 - Query in XML format\n";
	print "0 - Quit\n";

	$number = <STDIN>;
	chomp($number);
	print "\n";

	queryxml() if $number == 8;
	queryuri() if $number == 7;
	addserver() if $number == 2;
} while $number ne "0";


sub splituri {
	my $uri = shift(@_);
	$uri =~ s/\///g;
	@p = split(/\:/, $uri);
	$proto = $p[0];
	$host = $p[1];
	$port = $p[2];
	return ($proto, $host, $port);
}

sub queryuri {
	print "Enter URI to query on $uri\n";
	print "Example: query://ggz/connection/0.0.4\n";
	my $quri = <STDIN>;
	chomp($quri);
	doquery($quri);
}

sub queryxml {
	print "Enter XML statement to query on $uri\n";
	print "Example: <?xml version=\"1.0\"?><query class=\"ggz\" type=\"connection\">0.0.5pre</query>\n";
	my $query = <STDIN>;
	chomp($query);
	doquery($query);
}

sub addserver {
	print "Enter URI of GGZ server to add (ggz://jzaun.com:5688):\n";
	my $xuri = <STDIN>;
	chomp $xuri;
	print "Enter priority (1-100):\n";
	my $prio = <STDIN>;
	chomp $prio;
	print "Enter version (0.0.5pre):\n";
	my $version = <STDIN>;
	chomp $version;
	doquery("<?xml version=\"1.0\"?><update class=\"ggz\" type=\"connection\" username=\"$username\" password=\"$password\"><option name=\"version\">$version</option><option name=\"preference\">$prio</option><uri>$xuri</uri></update>");
}

sub doquery {
	my $query = shift(@_);

	$conn = new Net::Telnet(Host => $host, Port => $port) || nothing();
	if(!$conn) {
		print "ERROR: $uri unreachable\n";
		return;
	}

	print "<<<<<<<<<\n";
	print "PUT: $query\n";
	$conn->print($query);
	$data = $conn->get(Timeout => 5);
	chomp($data);
	print "GET: $data\n";
	print ">>>>>>>>>\n";
}

sub nothing {
}

