#!/usr/bin/perl
#
# Outputs a XML stream of currently running games
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

use strict;
use IO::Socket;

my $proto = getprotobyname("tcp");
my $port = 8080;
my $addr = sockaddr_in($port, INADDR_ANY);
socket(SERVER, PF_INET, SOCK_STREAM, $proto);
bind(SERVER, $addr);
listen(SERVER, SOMAXCONN);
setsockopt(SERVER, SOL_SOCKET, SO_REUSEADDR, 1);

my %server;
my %gamehost;
my $loading;

sub network{
	my $sockaddr = accept(HANDLER, SERVER);

	print HANDLER "<?xml version=\"1.0\"?>\n";
	print HANDLER "<ggzserver>\n";
	foreach my $room(keys(%server)){
		print HANDLER "<room name=\"$room\">\n";
		foreach my $game(keys(%{$server{$room}})){
			print HANDLER "<game name=\"$game\">\n";
			foreach my $table(keys(%{$server{$room}{$game}})){
				my @seatlist = keys(%{$server{$room}{$game}{$table}});
				if($#seatlist != -1){
					print HANDLER "<table id=\"$table\">\n";
					foreach my $seat(@seatlist){
						print HANDLER "<seat id=\"$seat\">";
						my $player = $server{$room}{$game}{$table}{$seat};
						print HANDLER "$player";
						print HANDLER "</seat>\n";
					}
					print HANDLER "</table>\n";
				}
			}
			print HANDLER "</game>\n";
		}
		print HANDLER "</room>\n";
	}
	print HANDLER "</ggzserver>\n";

	close(HANDLER);
}

STDOUT->autoflush(1);
$loading = 1;
print "Loading... ";

open(X, $ARGV[0]) or die;
while(1){
	while(<X>){
		chomp;
		my ($pid, $keyword, $message) = m/^\[(\d+)\]\:\ (\S+)\ ([\S\s]+)$/x;
		#my ($pid, $keyword, $message) =
		#	m/^\[(\d+)\]\:\ \S+\ \d+\ \d+\ \d+\:\d+\:\d+\ (\S+)\ ([\S\s]+)$/x;
		#print "<$pid:$keyword:$message>\n";
		if($keyword eq "TABLE_START"){
			my ($name, $game, $room) = $message =~ m/^\-\ (\S+)\ \S+\ \S+\ \S+\ \S+\ \S+\ (\S+)\ \S+\ (\S+)$/x;
			#print "table start: name=$name game=$game room=$room\n";
			#$server{$room}{$game} = {};
		}elsif($keyword eq "TABLE_JOIN"){
			my ($name, $seat, $table, $room, $game) = $message =~ m/^\-\ (\S+)\ \S+\ \S+\ (\d+)\ \S+\ \S+\ (\d+)\ \S+\ \S+\ (\S+)\ \S+\ \S+\ \S+\ (\S+)$/x;
			#print "table join: name=$name game=$game room=$room table=$table seat=$seat\n";
			$server{$room}{$game}{$table}{$seat} = $name;
			$gamehost{"$name:table"} = $table;
			$gamehost{"$name:room"} = $room;
		}elsif($keyword eq "TABLE_LEAVE"){
			my ($name, $seat, $table, $room, $game) = $message =~ m/^\-\ (\S+)\ \S+\ \S+\ (\d+)\ \S+\ \S+\ (\d+)\ \S+\ \S+\ (\S+)\ \S+\ \S+\ \S+\ (\S+)$/x;
			#print "table leave: name=$name game=$game room=$room table=$table seat=$seat\n";
			$server{$room}{$game}{$table}{$seat} = "";
		}elsif($keyword eq "TABLE_END"){
			my ($game, $name) = $message =~ m/^\-\ \S+\ \S+\ (\S+)\ \S+\ \S+\ (\S+)\ \S+\ \S+$/x;
			#print "table end: name=$name game=$game\n";
			my $table = $gamehost{"$name:table"};
			my $room = $gamehost{"$name:room"};
			$server{$room}{$game}{$table} = {};
		}
	}
	if($loading){
		$loading = 0;
		print "done\n";
	}
	sleep(1);
	network();
	seek(X, 0, 1);
}
close(X);

close(SERVER);

