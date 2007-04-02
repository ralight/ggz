#!/usr/bin/env perl
#
# Test client for the GGZ Community REST API
# REST client handler code

package ggzapi_rest;

use strict;
use LWP::UserAgent;
use XML::DOM;
use Term::Shell;

my %authcache;

sub call($){
	my $req = shift(@_);

	my $ua = LWP::UserAgent->new;
	if($authcache{'retry'}){
		$ua->credentials("api.ggzcommunity:80", "'ggzapi'",
			$authcache{'user'}, $authcache{'password'});
	}
	$ua->agent('GGZ Community Test Client');

	my $res = $ua->request($req);

	if($res->is_success){
		print "Success!\n";
		print $res->content, "\n";

		delete $authcache{'retry'};
		delete $authcache{'asked'};
	}else{
		print "Error!\n";
		print $res->status_line, "\n";

		if($res->code == 401){
			if(!$authcache{'retry'}){
				my $user = $authcache{'user'};
				my $password = $authcache{'password'};

				if(!$password){
					print "Authentication is needed, enter credentials!\n";

					my $shell = Term::Shell->new;
					$user = $shell->prompt("Username [previous: $user]: ", $user);
					$password = $shell->prompt("Password: ", $password);

					$authcache{'user'} = $user;
					$authcache{'password'} = $password;
					$authcache{'asked'} = 1;
				}else{
					print "Authentication is needed, retrying...\n";
				}

				$authcache{'retry'} = 1;
				return call($req);
				# FIXME: LWP then tries unauthenticated again, traffic overhead
			}else{
				print "Giving up :(\n";

				delete $authcache{'retry'};
				delete $authcache{'password'};

				if(!$authcache{'asked'}){
					return call($req);
				}
			}
		}
	}

	return $res;
}

sub call_players(){
	my $req = HTTP::Request->new(GET => "http://api.ggzcommunity/api/players");
	my $res = call($req);

	if(!$res->is_success){
		return;
	}

	# FIXME: XML::Parser/XML::DOM::Parser die if document isn't not wellformed
	my $parser = new XML::DOM::Parser;
	my $doc = $parser->parse($res->content);
	my $root = $doc->getDocumentElement;

	my $rootname = $root->getTagName;
	print "XML root: $rootname\n";

	if($rootname eq "players"){
		print "Received [players] message\n";

		# FIXME: fake list
		return ("player1", "player2");
	}else{
		print "Error: Unknown message\n";
	}
}

sub call_player($){
	my $player = shift(@_);

	my $req = HTTP::Request->new(POST => "http://api.ggzcommunity/api/players/$player");
	$req->content_type("application/ggzapi+xml");
	$req->content("<foo/>");
	my $res = call($req);

	if(!$res->is_success){
		return;
	}
}

1;

