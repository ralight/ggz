#!/usr/bin/perl
#
# Script to register a GGZ server with a GGZ metaserver
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

use strict;
use IO::Socket;

my $version = "0.0.8+"; # ggz server version
my $preference = "50"; # should be 50 for non-official servers
my $location = "*/*"; # city and country
my $speed = "102400"; # kbit/s

my $username = ""; # metaserver authorization username
my $password = ""; # metaserver authorization password
my $hostname = `hostname --fqdn`; # ggz server hostname
my $metaserver = "live.ggzgamingzone.org"; # metaserver hostname

my $sendstring = "<?xml version=\"1.0\"?>";
$sendstring .= "<update mode=\"add\" class=\"ggz\" type=\"connection\" username=\"$username\" password=\"$password\">";
$sendstring .= "<option name=\"uri\">ggz://$hostname</option>";
$sendstring .= "<option name=\"mode\">add</option>";
$sendstring .= "<option name=\"version\">$version</option>";
$sendstring .= "<option name=\"preference\">$preference</option>";
$sendstring .= "<option name=\"location\">$location</option>";
$sendstring .= "<option name=\"speed\">$speed</option>";
$sendstring .= "</update>";

my $proto = getprotobyname("tcp");
my $ip = gethostbyname($metaserver) or die "Metaserver host $metaserver not found";
my $port = 8080;
my $addr = sockaddr_in(15689, $ip);
socket(SERVER, PF_INET, SOCK_STREAM, $proto);
connect(SERVER, $addr) or die "Connection to metaserver $metaserver not possible";

print SERVER $sendstring;
SERVER->autoflush();

my $answer = <SERVER>;
my $answercode = $answer;
$answercode =~ s/.*<status>(.*)<\/status>.*\n/$1/;
print "$answercode\n";


