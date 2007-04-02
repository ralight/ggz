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

sub prompt_str(){
	"GGZ» ";
}

sub help_players(){}
sub smry_players(){
	"List all player names (GET)";
}
sub run_players(){
	shift(@_);
	@players = ggzapi_rest::call_players();
}

sub help_player(){}
sub smry_player(){
	"List a player entry (GET) or add/edit player (POST/PUT/DELETE)";
}
sub run_player(){
	shift(@_);
	my $arg_player = shift(@_);
	if($arg_player){
		ggzapi_rest::call_player($arg_player);
	}else{
		print "Syntax error!\n";
	}
}
sub comp_player(){
	shift(@_);
	my $word = shift(@_);

	my @list = @players;
	if($#list == -1){
		push @list, "<playername>";
	}

	return @list;
}

sub run_(){}

1;

