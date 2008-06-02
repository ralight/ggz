<?php

$ret = @include_once(".htconf");

if (!$dbport) :
	$dbport = 5432;
endif;

if ($ret) :
	$conn = @pg_connect("host=$dbhost dbname=$dbname user=$dbuser password=$dbpass port=$dbport");
else :
	$conn = null;
endif;

if (!$conn) :
	include_once("header.inc");
	include_once("unconfigured.inc");
	include_once("footer.inc");
	exit;
endif;

?>
