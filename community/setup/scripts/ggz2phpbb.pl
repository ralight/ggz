#!/usr/bin/perl
#
# Synchronizes the phpBB user database from GGZ
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

use strict;
use DBI;
use Digest::MD5;

# connection parameters for phpBB

my $phpbb_type = "mysql";
my $phpbb_host = "localhost";
my $phpbb_name = "phpbb";
my $phpbb_user = "xxx";
my $phpbb_pass = "xxx";

## connection parameters for GGZ

my $ggz_type = "Pg";
my $ggz_host = "localhost";
my $ggz_name = "ggz";
my $ggz_user = "xxx";
my $ggz_pass = "xxx";

### should all GGZ players join a group?

my $groupid = -1;

### end of configuration

my $ggz_conn = DBI->connect("DBI:$ggz_type:host=$ggz_host;dbname=$ggz_name;user=$ggz_user;password=$ggz_pass");
my $phpbb_conn = DBI->connect("DBI:$phpbb_type:host=$phpbb_host;dbname=$phpbb_name;user=$phpbb_user;password=$phpbb_pass");

my ($id, $name, $password, $email);

my $res = $ggz_conn->prepare("SELECT handle, password, email FROM users");
$res->execute();
$res->bind_columns(undef, \$name, \$password, \$email);

my $res2 = $phpbb_conn->prepare("SELECT MAX(user_id) FROM phpbb_users");
$res2->execute();
$res2->bind_columns(undef, \$id);
if($res2->fetch()){
	# ok
}else{
	$id = -1;
}
$res2->finish();

while($res->fetch()){
	my $res2 = $phpbb_conn->prepare("SELECT username FROM phpbb_users WHERE username = '$name'");
	$res2->execute();
	if($res2->fetch()){
		# already present
	}else{
		my $md5pass = Digest::MD5::md5_hex($password);
		if($email eq "N/A"){
			$email = "";
		}
		$id += 1;

		$res2 = $phpbb_conn->prepare("INSERT INTO phpbb_users " .
			"(user_id, username, user_password, user_email) VALUES " .
			"($id, '$name', '$md5pass', '$email')");
		$res2->execute();

		if($groupid != -1){
			$res2 = $phpbb_conn->prepare("INSERT INTO phpbb_user_group " .
				"(group_id, user_id, user_pending) VALUES " .
				"($groupid, $id, 0)");
			$res2->execute();
		}

		print "=> $name ($email)\n";
	}
}

$phpbb_conn->disconnect();
$ggz_conn->disconnect();


