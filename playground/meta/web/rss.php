<?php

$res = pg_exec($conn, "SELECT * FROM games WHERE key = '$gamekey'");

$name = pg_result($res, 0, "name");
$logo = pg_result($res, 0, "logo");
$homepage = pg_result($res, 0, "homepage");
$zone = pg_result($res, 0, "zone");

header("Content-type: application/rss+xml");

echo "<?xml version=\"1.0\"?>\n";
echo "<metacle>\n";

$res = pg_exec($conn, "SELECT * FROM gameservers WHERE key = '$gamekey'");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$uri = pg_result($res, $i, "uri");
	$version = pg_result($res, $i, "version");
	$state = pg_result($res, $i, "state");
	$topic = pg_result($res, $i, "topic");
	$players = pg_result($res, $i, "players");
	$available = pg_result($res, $i, "available");

	echo "<gameserver gamename=\"$name\">\n";
	echo "<uri>$uri</uri>\n";
	echo "<topic>$topic</topic>\n";
	echo "</gameserver>\n";
}

echo "</metacle>\n";
?>
