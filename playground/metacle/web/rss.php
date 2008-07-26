<?php

$res = pg_exec($conn, "SELECT * FROM games WHERE key = '$gamekey'");

$name = pg_result($res, 0, "name");
$logo = pg_result($res, 0, "logo");
$homepage = pg_result($res, 0, "homepage");
$zone = pg_result($res, 0, "zone");

if ($format == "rss") :
	header("Content-type: application/rss+xml");
else :
	header("Content-type: application/metacle");
endif;

echo "<?xml version='1.0'?>\n";
if ($format == "rss") :
	echo "<rss version='2.0'>\n";
	echo "<channel>\n";
	echo "<title>Metacle RSS feed for $gamekey</title>\n";
	echo "<link>http://metacle.ggzgamingzone.org/</link>\n";
	echo "<description>\n";
	echo "The Metacle - metaserver aggregation for games and gaming centres\n";
	echo "</description>\n";
else :
	echo "<metacle>\n";
endif;

$res = pg_exec($conn, "SELECT * FROM gameservers WHERE key = '$gamekey'");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$uri = pg_result($res, $i, "uri");
	$version = pg_result($res, $i, "version");
	$state = pg_result($res, $i, "state");
	$topic = pg_result($res, $i, "topic");
	$players = pg_result($res, $i, "players");
	$available = pg_result($res, $i, "available");

	$date = date("r");
	$hash = md5($uri);

	if ($format == "rss") :
		echo "<item>\n";
		echo "<guid>http://metacle.ggzgamingzone.org/$gamekey-$i-$hash</guid>\n";
		echo "<pubDate>$date</pubDate>\n";
		echo "<title>$name</title>\n";
		echo "<link>$uri</link>\n";
		echo "<description>$topic</description>\n";
		echo "</item>\n";
	else :
		echo "<gameserver gamename=\"$name\">\n";
		echo "<uri>$uri</uri>\n";
		echo "<topic>$topic</topic>\n";
		echo "</gameserver>\n";
	endif;
}

if ($format == "rss") :
	echo "</channel>\n";
	echo "</rss>\n";
else :
	echo "</metacle>\n";
endif;

?>
