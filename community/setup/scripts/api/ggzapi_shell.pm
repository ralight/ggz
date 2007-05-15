#!/usr/bin/env perl
#
# Test client for the GGZ Community REST API
# Shell for the input of commands

package ggzapi_shell;

use strict;
use Term::Shell;
use base qw(Term::Shell);
use ggzapi_rest;

my @players;
my @statisticsgames;
my @teams;

sub prompt_str(){
	"GGZ API» ";
}

sub help_players(){}
sub smry_players(){
	"List all player names (GET)";
}
sub run_players(){
	shift(@_);
	my $arg_method = shift(@_);
	if($arg_method eq "GET"){
		@players = ggzapi_rest::call_players();
	}else{
		print "Missing method name!\n";
	}
}

sub help_player(){}
sub smry_player(){
	"List a player entry (GET) or add/edit player (POST/PUT/DELETE)";
}
sub run_player(){
	my $self = shift(@_);
	my $arg_method = shift(@_);
	my $arg_player = shift(@_);
	if(!$arg_player){
		print "Syntax error (player <OP> <playername>)!\n";
		return;
	}
	my %playerinfo;
	if($arg_method eq "GET"){
			ggzapi_rest::call_player($arg_player, $arg_method, \%playerinfo);
	}elsif(($arg_method eq "POST") or ($arg_method eq "PUT")){
			$playerinfo{'password'} = $self->prompt("* Player password: ");
			$playerinfo{'email'} = $self->prompt("* Player email: ");
			$playerinfo{'realname'} = $self->prompt("* Player realname: ");
			$playerinfo{'photo'} = $self->prompt("* Player photo: ");
			ggzapi_rest::call_player($arg_player, $arg_method, \%playerinfo);
	}elsif($arg_method eq "DELETE"){
			ggzapi_rest::call_player($arg_player, $arg_method, \%playerinfo);
	}else{
		print "Invalid method name!\n";
	}
}
sub comp_player(){
	shift(@_);
	my $word = shift(@_);
	my $line = shift(@_);
	my @words = split(/ /, $line);
	my $position = $#words;
	if($word ne ""){
		$position = $position - 1;
	}

	my @list;
	if($position == 0){
		@list = ("GET", "POST", "PUT", "DELETE");
	}elsif($position == 1){
		@list = @players;
		if($#list == -1){
			push @list, "<playername>";
		}
	}

	return @list;
}

sub help_statistics_games(){}
sub smry_statistics_games(){
	"List all game names available in the statistics database (GET)";
}
sub run_statistics_games(){
	shift(@_);
	my $arg_method = shift(@_);
	if($arg_method eq "GET"){
		@statisticsgames = ggzapi_rest::call_statistics_games();
	}else{
		print "Missing method name!\n";
	}
}

sub help_statistics_game(){}
sub smry_statistics_game(){
	"List the statistics for a single game type (GET)";
}
sub run_statistics_game(){
	my $self = shift(@_);
	my $arg_method = shift(@_);
	my $arg_game = shift(@_);
	if(!$arg_game){
		print "Syntax error (statistics_game <OP> <gamename>)!\n";
		return;
	}
	if($arg_method eq "GET"){
		ggzapi_rest::call_statistics_game($arg_game);
	}else{
		print "Invalid method name!\n";
	}
}
sub comp_statistics_game(){
	shift(@_);
	my $word = shift(@_);
	my $line = shift(@_);
	my @words = split(/ /, $line);
	my $position = $#words;
	if($word ne ""){
		$position = $position - 1;
	}

	my @list;
	if($position == 0){
		@list = ("GET", "POST", "PUT", "DELETE");
	}elsif($position == 1){
		@list = @statisticsgames;
		if($#list == -1){
			push @list, "<gamename>";
		}
	}

	return @list;
}

sub help_teams(){}
sub smry_teams(){
	"List all team names (GET)";
}
sub run_teams(){
	shift(@_);
	my $arg_method = shift(@_);
	if($arg_method eq "GET"){
		@teams = ggzapi_rest::call_teams();
	}else{
		print "Missing method name!\n";
	}
}

sub help_team(){}
sub smry_team(){
	"List a team entry (GET) or add/edit team (POST/PUT/DELETE)";
}
sub run_team(){
	my $self = shift(@_);
	my $arg_method = shift(@_);
	my $arg_team = shift(@_);
	if(!$arg_team){
		print "Syntax error (team <OP> <teamname>)!\n";
		return;
	}
	my %teaminfo;
	if($arg_method eq "GET"){
			ggzapi_rest::call_team($arg_team, $arg_method, \%teaminfo);
	}elsif(($arg_method eq "POST") or ($arg_method eq "PUT")){
			$teaminfo{'fullname'} = $self->prompt("* Full name of team: ");
			$teaminfo{'homepage'} = $self->prompt("* Homepage: ");
			ggzapi_rest::call_team($arg_team, $arg_method, \%teaminfo);
	}elsif($arg_method eq "DELETE"){
			ggzapi_rest::call_team($arg_team, $arg_method, \%teaminfo);
	}else{
		print "Invalid method name!\n";
	}
}
sub comp_team(){
	shift(@_);
	my $word = shift(@_);
	my $line = shift(@_);
	my @words = split(/ /, $line);
	my $position = $#words;
	if($word ne ""){
		$position = $position - 1;
	}

	my @list;
	if($position == 0){
		@list = ("GET", "POST", "PUT", "DELETE");
	}elsif($position == 1){
		@list = @players;
		if($#list == -1){
			push @list, "<teamname>";
		}
	}

	return @list;
}

sub help_team_player(){}
sub smry_team_player(){
	"List members of a team (GET) or add/edit team players (POST/PUT/DELETE)";
}
sub run_team_player(){
	my $self = shift(@_);
	my $arg_method = shift(@_);
	my $arg_team = shift(@_);
	my $arg_teamplayer = shift(@_);
	if((!$arg_team) || (!$arg_teamplayer)){
		print "Syntax error (team_player <OP> <teamname> <teamplayername>)!\n";
		return;
	}
	my %teamplayerinfo;
	if($arg_method eq "GET"){
			ggzapi_rest::call_team_player($arg_team, $arg_teamplayer, $arg_method, \%teamplayerinfo);
	}elsif(($arg_method eq "POST") or ($arg_method eq "PUT")){
			$teamplayerinfo{'role'} = $self->prompt("* Role of team player: ");
			ggzapi_rest::call_team_player($arg_team, $arg_teamplayer, $arg_method, \%teamplayerinfo);
	}elsif($arg_method eq "DELETE"){
			ggzapi_rest::call_team_player($arg_team, $arg_teamplayer, $arg_method, \%teamplayerinfo);
	}else{
		print "Invalid method name!\n";
	}
}
sub comp_team_player(){
	shift(@_);
	my $word = shift(@_);
	my $line = shift(@_);
	my @words = split(/ /, $line);
	my $position = $#words;
	if($word ne ""){
		$position = $position - 1;
	}

	my @list;
	if($position == 0){
		@list = ("GET", "POST", "PUT", "DELETE");
	}elsif($position == 1){
		@list = @teams;
		if($#list == -1){
			push @list, "<teamname>";
		}
	}elsif($position == 2){
		@list = @players;
		if($#list == -1){
			push @list, "<teamplayername>";
		}
	}

	return @list;
}

sub run_(){}

1;

