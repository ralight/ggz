<?php

// =============================================================
// GGZ Community Web Services API
// Copyright (C) 2007 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//
// For a permanent reference installation, please refer to
// http://api.ggzcommunity.org/
// =============================================================

// -------------------------------------------------------------
// Read the configuration file
// This will give us $ggzincludefile and $debug

$ret = @include_once(".htconf.php");
if (!$ret) :
	header("Content-type: text/plain");
	header("HTTP/1.1 500 Totally unconfigured");
	echo "Error: File .htconf.php not found";
	exit;
endif;

// -------------------------------------------------------------
// Read the include file
// Defines the 'parseconfigfile' function

$ret = @include_once(".htinclude.php");
if (!$ret) :
	header("Content-type: text/plain");
	header("HTTP/1.1 500 Totally misinstalled");
	echo "Error: File .htinclude.php not found";
	exit;
endif;

// -------------------------------------------------------------
// Parse the ggzd config file
// Gives us all the db* variables so we can connect to the DB

if ($ggzconfigfile) :
	$ini = parseconfigfile($ggzconfigfile);
	if (!$ini) :
		header("Content-type: text/plain");
		header("HTTP/1.1 500 The ggzd config file is screwed");
		echo "Error: File $ggzconfigfile not found";
		exit;
	endif;
	$dbhost = $ini["General"]["DatabaseHost"];
	$dbname = $ini["General"]["DatabaseName"];
	$dbuser = $ini["General"]["DatabaseUsername"];
	$dbpass = $ini["General"]["DatabasePassword"];
endif;

// -------------------------------------------------------------
// Connect to the database

$conn = @pg_connect("host=$dbhost dbname=$dbname user=$dbuser password=$dbpass");
if (!$conn) :
	header("Content-type: text/plain");
	header("HTTP/1.1 500 We lost the database");
	echo "Error: Could not connect to the database";
	exit;
endif;

// -------------------------------------------------------------
// So far so good... start working on the input here

$uri = $_SERVER["REQUEST_URI"];
$method = $_SERVER["REQUEST_METHOD"];
$resource = str_replace("/api/", "", $uri);

$resourceparts = explode("/", $resource);
$topresource = $resourceparts[0];
$subresource = $resourceparts[1];
$subsubresource = $resourceparts[2];

$error = 0;
$autherror = 0;

$input = "";
$authenticated = 0;

if (($method == "POST") || ($method == "PUT")) :
	$input = file_get_contents("php://input");
	$contenttype = $_SERVER["CONTENT_TYPE"];

	if ($contenttype != "application/ggzapi+xml") :
		header("Content-type: text/plain");
		header("HTTP/1.1 500 It's all ggzapi+xml, man");
		echo "Error: Wrong MIME type for post";
		exit;
	endif;
endif;

if (isset($_SERVER["PHP_AUTH_USER"])) :
	$user = $_SERVER["PHP_AUTH_USER"];
	$password = $_SERVER["PHP_AUTH_PW"];

	$res = pg_exec($conn, "SELECT * FROM users WHERE handle = '$user' AND password = '$password'");
	if (pg_numrows($res) == 1) :
		$authenticated = 1;
	endif;
endif;

// -------------------------------------------------------------
// Resource and method handling

ob_start();

if ($resource == "") :
	echo "GGZ Community Web Services API";
elseif ($topresource == "players") :
	if ($subresource == "") :
		if ($method == "GET") :
			$res = pg_exec($conn, "SELECT handle FROM users");

			echo "<players>";
			for($i = 0; $i < pg_numrows($res); $i++)
			{
				$playername = pg_result($res, $i, 0);
				echo "<player name='$playername'/>";
			}
			echo "</players>";
		else :
			$error = 1;
		endif;
	else :
		$playername = $subresource;
		if ($playername != $user) :
			$authenticated = 0;
		endif;

		if ($method == "GET") :
			$res = pg_exec($conn, "SELECT email, name FROM users WHERE handle = '$playername'");

			if (pg_numrows($res) == 1) :
				$email = pg_result($res, 0, 0);
				$realname = pg_result($res, 0, 1);

				$res = pg_exec($conn, "SELECT photo FROM userinfo WHERE handle = '$playername'");
				if (pg_numrows($res) == 1) :
					$photo = pg_result($res, 0, 0);
				else :
					$photo = "";
				endif;

				echo "<player name='$playername'>";
				echo "<email>$email</email>";
				echo "<realname>$realname</realname>";
				if ($photo) :
					echo "<photo>$photo</photo>";
				endif;
				echo "</player>";
			else:
				$error = 1;
			endif;
		elseif ($method == "POST") :
			$doc = new DOMDocument();
			$doc->loadXML($input);
			$root = $doc->documentElement;
			if ($root->tagName == "player") :
				$nodes = $root->getElementsByTagName("*");
				foreach ($nodes as $node)
				{
					#echo "++ ", $node->nodeName, "\n";
					#echo "-- ", $node->nodeValue, "\n";

					if ($node->nodeName == "password") :
						$password = $node->nodeValue;
					elseif ($node->nodeName == "email") :
						$email = $node->nodeValue;
					elseif ($node->nodeName == "realname") :
						$realname = $node->nodeValue;
					elseif ($node->nodeName == "photo") :
						$photo = $node->nodeValue;
					endif;
				}

				$stamp = time();
				# jointable, launchtable, roomslogin
				$perms = 0x07;

				$res = pg_exec($conn,
					"INSERT INTO users (handle, password, name, email, firstlogin, permissions) VALUES ('$playername', '$password', '$realname', '$email', $stamp, $perms)");

				$res = pg_exec($conn,
					"INSERT INTO userinfo (handle, photo) VALUES ('$playername', '$photo')");
				# FIXME: check against malicious input
				# FIXME: check database result
				# FIXME: check duplicates, let db do it?
			else :
				$error = 1;
			endif;
		elseif ($method == "PUT") :
			if ($authenticated) :
				$doc = new DOMDocument();
				$doc->loadXML($input);
				$root = $doc->documentElement;
				if ($root->tagName == "player") :
					$nodes = $root->getElementsByTagName("*");
					foreach ($nodes as $node)
					{
						if ($node->nodeName == "password") :
							$password = $node->nodeValue;
						elseif ($node->nodeName == "email") :
							$email = $node->nodeValue;
						elseif ($node->nodeName == "realname") :
							$realname = $node->nodeValue;
						elseif ($node->nodeName == "photo") :
							$photo = $node->nodeValue;
						endif;
					}

					$res = pg_exec($conn,
						"UPDATE users SET password = '$password', email = '$email', name = '$realname' " .
						"WHERE handle = '$playername'");

					$res = pg_exec($conn,
						"UPDATE userinfo SET photo = '$photo' WHERE handle = '$playername'");
					# FIXME: check against malicious input
					# FIXME: check database result
					# FIXME: check duplicates, let db do it?
				else :
					$error = 1;
				endif;
			else :
				$error = 1;
				$autherror = 1;
			endif;
		elseif ($method == "DELETE") :
			if ($authenticated) :
				$res = pg_exec($conn, "DELETE FROM users WHERE handle = '$playername'");
				$res = pg_exec($conn, "DELETE FROM userinfo WHERE handle = '$playername'");
				# FIXME: check against malicious input
				# FIXME: check database result
				# FIXME: check presence etc.
			else :
				$error = 1;
				$autherror = 1;
			endif;
		else :
			$error = 1;
		endif;
	endif;
elseif ($topresource == "statistics") :
	if ($subresource == "games") :
		if ($subsubresource == "") :
			if ($method == "GET") :
				$res = pg_exec($conn, "SELECT DISTINCT game FROM stats");

				echo "<games>";
				for($i = 0; $i < pg_numrows($res); $i++)
				{
					$gamename = pg_result($res, $i, 0);
					echo "<game name='$gamename'/>";
				}
				echo "</games>";
			else :
				$error = 1;
			endif;
		else :
			if ($method == "GET") :
				$gamename = $subsubresource;
				$res = pg_exec($conn, "SELECT handle, rating, highscore FROM stats WHERE game = '$gamename' ORDER BY ranking");

				echo "<statistics>";
				for($i = 0; $i < pg_numrows($res); $i++)
				{
					$playername = pg_result($res, $i, 0);
					$rating = pg_result($res, $i, 1);
					$highscore = pg_result($res, $i, 2);
					echo "<statsplayer name='$playername'>";
					echo "<rating>$rating</rating>";
					echo "</statsplayer>";
				}
				echo "</statistics>";
			else :
				$error = 1;
			endif;
		endif;
	else :
		$error = 1;
	endif;
elseif ($topresource == "teams") :
	if ($subresource == "") :
		if ($method == "GET") :
			$res = pg_exec($conn, "SELECT teamname FROM teams");

			echo "<teams>";
			for($i = 0; $i < pg_numrows($res); $i++)
			{
				$teamname = pg_result($res, $i, 0);
				echo "<team name='$teamname'/>";
			}
			echo "</teams>";
		else :
			$error = 1;
		endif;
	else :
		$teamname = $subresource;
		// FIXME: check against team founder, but only for subsubresource == ""

		if ($subsubresource == "") :
			if ($method == "GET") :
				$res = pg_exec($conn, "SELECT founder, foundingdate, fullname, homepage FROM teams WHERE teamname = '$teamname'");

				if (pg_numrows($res) == 1) :
					$founder = pg_result($res, 0, 0);
					$foundingdate = pg_result($res, 0, 1);
					$fullname = pg_result($res, 0, 1);
					$homepage = pg_result($res, 0, 1);

					echo "<team name='$teamname'>";
					echo "<founder>$founder</founder>";
					echo "<foundingdate>$foundingdate</foundingdate>";
					echo "<fullname>$fullname</fullname>";
					echo "<homepage>$homepage</homepage>";
					echo "</team>";
				else:
					$error = 1;
				endif;
			elseif ($method == "POST") :
				if ($authenticated) :
					$doc = new DOMDocument();
					$doc->loadXML($input);
					$root = $doc->documentElement;
					if ($root->tagName == "team") :
						$nodes = $root->getElementsByTagName("*");
						foreach ($nodes as $node)
						{
							if ($node->nodeName == "fullname") :
								$fullname = $node->nodeValue;
							elseif ($node->nodeName == "homepage") :
								$homepage = $node->nodeValue;
							endif;
						}

						$stamp = time();
						$founder = $user;

						$res = pg_exec($conn,
							"INSERT INTO teams (teamname, fullname, homepage, founder, foundingdate) VALUES ('$teamname', '$fullname', '$homepage', '$founder', $stamp)");

						# FIXME: check against malicious input
						# FIXME: check database result
						# FIXME: check duplicates, let db do it?
					else :
						$error = 1;
					endif;
				else :
					$error = 1;
					$autherror = 1;
				endif;
			elseif ($method == "PUT") :
				if ($authenticated) :
					$doc = new DOMDocument();
					$doc->loadXML($input);
					$root = $doc->documentElement;
					if ($root->tagName == "team") :
						$nodes = $root->getElementsByTagName("*");
						foreach ($nodes as $node)
						{
							if ($node->nodeName == "fullname") :
								$fullname = $node->nodeValue;
							elseif ($node->nodeName == "homepage") :
								$homepage = $node->nodeValue;
							endif;
						}

						$res = pg_exec($conn,
							"UPDATE teams SET fullname = '$fullname', homepage = '$homepage' " .
							"WHERE teamname = '$teamname'");

						# FIXME: check against malicious input
						# FIXME: check database result
						# FIXME: check duplicates, let db do it?
					else :
						$error = 1;
					endif;
				else :
					$error = 1;
					$autherror = 1;
				endif;
			elseif ($method == "DELETE") :
				if ($authenticated) :
					$res = pg_exec($conn, "DELETE FROM teams WHERE teamname = '$teamname'");
					# FIXME: check against malicious input
					# FIXME: check database result
					# FIXME: check presence etc.
				else :
					$error = 1;
					$autherror = 1;
				endif;
			else :
				$error = 1;
			endif;
		else :
			$teamplayername = $subsubresource;
			if ($method == "GET") :
				$res = pg_exec($conn, "SELECT role, entrydate FROM teammembers WHERE teamname = '$teamname' AND handle = '$teamplayername'");

				if (pg_numrows($res) == 1) :
					$role = pg_result($res, 0, 0);
					$entrydate = pg_result($res, 0, 1);

					echo "<teamplayer name='$teamname'>";
					echo "<role>$role</role>";
					echo "<entrydate>$entrydate</entrydate>";
					echo "</teamplayer>";
				else:
					$error = 1;
				endif;
			elseif ($method == "POST") :
				if ($authenticated) :
					$doc = new DOMDocument();
					$doc->loadXML($input);
					$root = $doc->documentElement;
					if ($root->tagName == "teamplayer") :
						$nodes = $root->getElementsByTagName("*");
						foreach ($nodes as $node)
						{
							if ($node->nodeName == "role") :
								$role = $node->nodeValue;
							endif;
						}

						$stamp = time();

						$res = pg_exec($conn,
							"INSERT INTO teammembers (teamname, handle, role, entrydate) VALUES ('$teamname', '$teamplayername', '$role', $stamp)");

						# FIXME: check against malicious input
						# FIXME: check database result
						# FIXME: check duplicates, let db do it?
					else :
						$error = 1;
					endif;
				else :
					$error = 1;
					$autherror = 1;
				endif;
			elseif ($method == "PUT") :
				if ($authenticated) :
					$doc = new DOMDocument();
					$doc->loadXML($input);
					$root = $doc->documentElement;
					if ($root->tagName == "teamplayer") :
						$nodes = $root->getElementsByTagName("*");
						foreach ($nodes as $node)
						{
							if ($node->nodeName == "role") :
								$role = $node->nodeValue;
							endif;
						}

						$res = pg_exec($conn,
							"UPDATE teammembers SET role = '$role' " .
							"WHERE teamname = '$teamname' AND handle = '$teamplayername'");

						# FIXME: check against malicious input
						# FIXME: check database result
						# FIXME: check duplicates, let db do it?
					else :
						$error = 1;
					endif;
				else :
					$error = 1;
					$autherror = 1;
				endif;
			elseif ($method == "DELETE") :
				if ($authenticated) :
					$res = pg_exec($conn, "DELETE FROM teammembers WHERE teamname = '$teamname' AND handle = '$teamplayername'");
					# FIXME: check against malicious input
					# FIXME: check database result
					# FIXME: check presence etc.
				else :
					$error = 1;
					$autherror = 1;
				endif;
			else :
				$error = 1;
			endif;
		endif;
	endif;
else:
	$error = 1;
endif;

// -------------------------------------------------------------
// Finish the script

if (!$error) :
	if (!$debug) :
		header("Content-type: application/ggzapi+xml");
	else :
		header("Content-type: text/plain");
	endif;
	header("HTTP/1.1 200 The world is not enough");

	ob_end_flush();
elseif ($autherror) :
	header("Content-type: text/plain");
	header("WWW-Authenticate: Basic realm='ggzapi'");
	header("HTTP/1.1 401 Who are you anyway?");

	echo "The request requires authentication";
else :
	header("Content-type: text/plain");
	header("HTTP/1.1 501 There was an error");

	echo "Error: undefined resource or method ($resource, $method)";
endif;

?>