<?php
//
// Dynamic creation of a GHNS entry list (stuff.xml)
// Copyright (C) 2004 Josef Spillner <josef@kstuff.org>
// Pusblished under GNU GPL conditions.

include("../.htconf");

$conn = pg_connect("host=$conf_host dbname=$conf_name user=$conf_user password=$conf_pass");

$query = "SELECT * FROM directory";

$res = pg_exec($conn, $query);

echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
echo "<!DOCTYPE knewstuff SYSTEM \"knewstuff.dtd\">\n";
echo "<knewstuff>\n";

function autoselect($var, $type, $lang)
{
	global $conn;

	$res2 = pg_exec($conn, "SELECT * FROM contents " .
		"WHERE id = $var AND type = '$type' AND language = '$lang'");
	if (($res2) && (pg_numrows($res2) == 1)) :
		$var = pg_result($res2, 0, "content");
	else :

		$res2 = pg_exec($conn, "SELECT * FROM contents " .
			"WHERE id = $var AND type = '$type' LIMIT 1");
		if (($res2) && (pg_numrows($res2) == 1)) :
			$var = pg_result($res2, 0, "content");
		else :
			$var = "Not available.";
		endif;

	endif;
	return $var;
}

for ($i = 0; $i < pg_numrows($res); $i++)
{
	$name = pg_result($res, $i, "name");
	$version = pg_result($res, $i, "version");
	$author = pg_result($res, $i, "author");
	$releasedate = pg_result($res, $i, "releasedate");

	$licence = pg_result($res, $i, "licence");
	$release = pg_result($res, $i, "release");
	$rating = pg_result($res, $i, "rating");
	$downloads = pg_result($res, $i, "downloads");
	$type = pg_result($res, $i, "type");

	$summary = pg_result($res, $i, "summary");
	$payload = pg_result($res, $i, "payload");
	$preview = pg_result($res, $i, "preview");
	$summary = autoselect($summary, "summary", $lang);
	$payload = autoselect($payload, "payload", $lang);
	$preview = autoselect($preview, "preview", $lang);

	echo "  <stuff>\n";

	echo "    <name>$name</name>\n";
	echo "    <type>$type</type>\n";
	echo "    <author>$author</author>\n";
	echo "    <licence>$licence</licence>\n";
	echo "    <summary>$summary</summary>\n";
	echo "    <summary lang=\"pt_BR\">$summary pt_BR</summary>\n";
	echo "    <summary lang=\"en_US\">$summary en_US</summary>\n";
	echo "    <version>$version</version>\n";
	echo "    <release>$release</release>\n";
	echo "    <releasedate>$releasedate</releasedate>\n";
	echo "    <preview>$preview</preview>\n";
	echo "    <payload>$payload</payload>\n";
	echo "    <rating>$rating</rating>\n";
	echo "    <downloads>$downloads</downloads>\n";

	echo "  </stuff>\n";
}

echo "</knewstuff>\n";

?>

