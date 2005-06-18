#!/usr/bin/perl
#
# Planet blog aggregation configuration from GGZ user database
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

use strict;
use DBI;

## connection parameters for GGZ

my $ggz_type = "Pg";
my $ggz_host = "localhost";
my $ggz_name = "ggz";
my $ggz_user = "xxx";
my $ggz_pass = "xxx";

### end of configuration

my $ggz_conn = DBI->connect("DBI:$ggz_type:host=$ggz_host;dbname=$ggz_name;user=$ggz_user;password=$ggz_pass");
$ggz_conn || die;

my ($name, $blogfeed);

my $res = $ggz_conn->prepare("SELECT handle, blogfeed FROM userinfo WHERE blogfeed IS NOT NULL AND blogfeed <> ''");
$res->execute();
$res->bind_columns(undef, \$name, \$blogfeed);

while($res->fetch()){
	print "[$blogfeed]\n";
	print "name = $name\n";
	print "\n";
}

$ggz_conn->disconnect();


