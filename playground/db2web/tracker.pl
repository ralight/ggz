#!/usr/bin/perl

use strict;
use IO::Socket;

my $proto = getprotobyname("tcp");
my $port = 8080;
my $addr = sockaddr_in($port, INADDR_ANY);
socket(SERVER, PF_INET, SOCK_STREAM, $proto);
bind(SERVER, $addr);
listen(SERVER, SOMAXCONN);

my %server;
my %gamehost;

sub network{
	my $sockaddr = accept(HANDLER, SERVER);

	print HANDLER "<?xml version=\"1.0\"?>\n";
	print HANDLER "<ggzserver>\n";
	foreach my $room(keys(%server)){
		print HANDLER "<room name=\"$room\">\n";
		foreach my $game(keys(%{$server{$room}})){
			print HANDLER "<game name=\"$game\">\n";
			foreach my $table(keys(%{$server{$room}{$game}})){
				print HANDLER "<table id=\"$table\">\n";
				foreach my $seat(keys(%{$server{$room}{$game}{$table}})){
					print HANDLER "<seat id=\"$seat\">";
					my $player = $server{$room}{$game}{$table}{$seat};
					print HANDLER "$player";
					print HANDLER "</seat>\n";
				}
				print HANDLER "</table>\n";
			}
			print HANDLER "</game>\n";
		}
		print HANDLER "</room>\n";
	}
	print HANDLER "</ggzserver>\n";

	close(HANDLER);
}

open(X, $ARGV[0]);
while(1){
	while(<X>){
		chomp;
		my ($pid, $keyword, $message) = m/^\[(\d+)\]\:\ (\S+)\ ([\S\s]+)$/x;
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
	sleep(1);
	network();
	seek(X, 0, 1);
}
close(X);

close(SERVER);

