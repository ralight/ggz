#!/usr/bin/perl
#
# Synchronizes the phpBB user database from GGZ
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

use strict;
use DBI;
use Digest::MD5;

# connection parameters for phpBB

my $phpbb_type = "Pg";
my $phpbb_host = "localhost";
my $phpbb_name = "phpbb2";
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
$ggz_conn || die;

my $phpbb_conn = DBI->connect("DBI:$phpbb_type:host=$phpbb_host;dbname=$phpbb_name;user=$phpbb_user;password=$phpbb_pass");
$phpbb_conn || die;

my ($id, $name, $fullname, $password, $email);
my ($country);

my $res = $ggz_conn->prepare("SELECT handle, name, password, email FROM users");
$res->execute();
$res->bind_columns(undef, \$name, \$fullname, \$password, \$email);

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
	$res2 = $phpbb_conn->prepare("SELECT username FROM phpbb_users WHERE username = '$name'");
	$res2->execute();
	if($res2->fetch()){
		# already present
	}else{
		my $md5pass = Digest::MD5::md5_hex($password);
		if($email eq "N/A"){
			$email = "";
		}
		$id += 1;

		my $res3 = $ggz_conn->prepare("SELECT country FROM userinfo " .
			"WHERE handle = '$name'");
		$res3->execute();
		$res3->bind_columns(undef, \$country);
		if($res3->fetch()){
			# ok, user info found
		}else{
			$country = "";
		}
		$res3->finish();

		my $stamp = `date +%s`;

		$res3 = $phpbb_conn->prepare("INSERT INTO phpbb_users " .
			"(user_id, username, user_password, user_email, user_from, user_active, user_level, user_regdate) VALUES " .
			"($id, '$name', '$md5pass', '$email', '$country', 1, 0, $stamp)");
		$res3->execute();
		$res3->finish();

		if($groupid != -1){
			$res3 = $phpbb_conn->prepare("INSERT INTO phpbb_user_group " .
				"(group_id, user_id, user_pending) VALUES " .
				"($groupid, $id, 0)");
			$res3->execute();
			$res3->finish();
		}

		print "=> $name ($email) [$country]\n";
	}
	$res2->finish();
}

$phpbb_conn->disconnect();
$ggz_conn->disconnect();


