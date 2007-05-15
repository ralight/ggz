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
// Helper function to quote REST parameters in POST/PUT input data
// Since we use directory-style REST, we always have to add slashes
// to resource paths

function resourcequote($s)
{
	return addslashes($s);
}

function quote($s)
{
	if (!get_magic_quotes_gpc()) :
		return addslashes($s);
	else :
		return $s;
	endif;
}

// Database helper functions

function db_exec($conn, $query, $args)
{
	global $internalerror;

	// This is stolen from common/database.php
	$argcount = substr_count($query, "%^");
	if ($argcount > 0) :
		if ((!is_array($args)) || (count($args) != $argcount)) :
			return null;
		endif;
		for ($i = 0; $i < $argcount; $i++)
		{
			$query = substr_replace($query,
				quote($args[$i]),
				strpos($query, "%^"),
				2);
		}
	endif;

	$res = @pg_exec($conn, $query);
	if (!$res) :
		$internalerror = 1;
	endif;

	return $res;
}

function db_numrows($res)
{
	return @pg_numrows($res);
}

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
$topresource = resourcequote($resourceparts[0]);
$subresource = resourcequote($resourceparts[1]);
$subsubresource = resourcequote($resourceparts[2]);

$error = 0;
$autherror = 0;
$inputerror = 0;
$internalerror = 0;

$xmlroot = "";
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

	if ($input) :
		$doc = new DOMDocument();
		$doc->loadXML($input);
		$xmlroot = $doc->documentElement;
	endif;
endif;

if (isset($_SERVER["PHP_AUTH_USER"])) :
	$user = $_SERVER["PHP_AUTH_USER"];
	$password = $_SERVER["PHP_AUTH_PW"];

	$res = db_exec($conn,
		"SELECT * FROM users WHERE handle = '%^' AND password = '%^'",
		array($user, $password));
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
			$res = db_exec($conn, "SELECT handle FROM users", null);

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
			$res = db_exec($conn,
				"SELECT email, name FROM users WHERE handle = '%^'",
				array($playername));

			if (pg_numrows($res) == 1) :
				$email = pg_result($res, 0, 0);
				$realname = pg_result($res, 0, 1);

				$res = db_exec($conn,
					"SELECT photo FROM userinfo WHERE handle = '%^'",
					array($playername));
				if (db_numrows($res) == 1) :
					$photo = pg_result($res, 0, 0);
				else :
					$photo = "";
				endif;

				if (!$internalerror) :
					echo "<player name='$playername'>";
					echo "<email>$email</email>";
					echo "<realname>$realname</realname>";
					if ($photo) :
						echo "<photo>$photo</photo>";
					endif;
					echo "</player>";
				endif;
			else:
				$error = 1;
			endif;
		elseif ($method == "POST") :
			if (($xmlroot) && ($xmlroot->tagName == "player")) :
				$nodes = $xmlroot->getElementsByTagName("*");
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

				$stamp = time();
				define("PERM_JOIN_TABLE", 1);
				define("PERM_LAUNCH_TABLE", 2);
				define("PERM_ROOMS_LOGIN", 4);
				$perms = PERM_JOIN_TABLE + PERM_LAUNCH_TABLE + PERM_ROOMS_LOGIN;

				db_exec($conn,
					"INSERT INTO users (handle, password, name, email, firstlogin, permissions) " .
					"VALUES ('%^', '%^', '%^', '%^', %^, %^)",
					array($playername, $password, $realname, $email, $stamp, $perms));

				db_exec($conn,
					"INSERT INTO userinfo (handle, photo) VALUES ('%^', '%^')",
					array($playername, $photo));
				# FIXME: check duplicates, let db do it?
			else :
				$inputerror = 1;
			endif;
		elseif ($method == "PUT") :
			if ($authenticated) :
				if (($xmlroot) && ($xmlroot->tagName == "player")) :
					$nodes = $xmlroot->getElementsByTagName("*");
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

					db_exec($conn,
						"UPDATE users SET password = '%^', email = '%^', name = '%^' " .
						"WHERE handle = '%^'",
						array($password, $email, $realname, $playername));

					db_exec($conn,
						"UPDATE userinfo SET photo = '%^' WHERE handle = '%^'",
						array($photo, $playername));
					# FIXME: check duplicates, let db do it?
				else :
					$inputerror = 1;
				endif;
			else :
				$autherror = 1;
			endif;
		elseif ($method == "DELETE") :
			if ($authenticated) :
				db_exec($conn,
					"DELETE FROM users WHERE handle = '%^'",
					array($playername));
				db_exec($conn,
					"DELETE FROM userinfo WHERE handle = '%^'",
					array($playername));
				# FIXME: check presence etc.
			else :
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
				$res = db_exec($conn, "SELECT DISTINCT game FROM stats", null);

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
				$res = db_exec($conn, "SELECT handle, rating, highscore FROM stats " .
					"WHERE game = '%^' ORDER BY ranking",
					array($gamename));

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
			$res = db_exec($conn, "SELECT teamname FROM teams", null);

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
				$res = db_exec($conn,
					"SELECT founder, foundingdate, fullname, homepage FROM teams " .
					"WHERE teamname = '%^'",
					array($teamname));

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
					if (($xmlroot) && ($xmlroot->tagName == "team")) :
						$nodes = $xmlroot->getElementsByTagName("*");
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

						db_exec($conn,
							"INSERT INTO teams (teamname, fullname, homepage, founder, foundingdate) " .
							"VALUES ('%^', '%^', '%^', '%^', %^)",
							array($teamname, $fullname, $homepage, $founder, $stamp));

						# FIXME: check duplicates, let db do it?
					else :
						$inputerror = 1;
					endif;
				else :
					$autherror = 1;
				endif;
			elseif ($method == "PUT") :
				if ($authenticated) :
					if (($xmlroot) && ($xmlroot->tagName == "team")) :
						$nodes = $xmlroot->getElementsByTagName("*");
						foreach ($nodes as $node)
						{
							if ($node->nodeName == "fullname") :
								$fullname = $node->nodeValue;
							elseif ($node->nodeName == "homepage") :
								$homepage = $node->nodeValue;
							endif;
						}

						db_exec($conn,
							"UPDATE teams SET fullname = '%^', homepage = '%^' " .
							"WHERE teamname = '%^'",
							array($fullname, $homepage, $teamname));

						# FIXME: check duplicates, let db do it?
					else :
						$inputerror = 1;
					endif;
				else :
					$autherror = 1;
				endif;
			elseif ($method == "DELETE") :
				if ($authenticated) :
					db_exec($conn,
						"DELETE FROM teams WHERE teamname = '%^'",
						array($teamname));
					# FIXME: check presence etc.
				else :
					$autherror = 1;
				endif;
			else :
				$error = 1;
			endif;
		else :
			$teamplayername = $subsubresource;
			if ($method == "GET") :
				$res = db_exec($conn,
					"SELECT role, entrydate FROM teammembers " .
					"WHERE teamname = '%^' AND handle = '%^'",
					array($teamname, $teamplayername));

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
					if (($xmlroot) && ($xmlroot->tagName == "teamplayer")) :
						$nodes = $xmlroot->getElementsByTagName("*");
						foreach ($nodes as $node)
						{
							if ($node->nodeName == "role") :
								$role = $node->nodeValue;
							endif;
						}

						$stamp = time();

						db_exec($conn,
							"INSERT INTO teammembers (teamname, handle, role, entrydate) " .
							"VALUES ('%^', '%^', '%^', %^)",
							array($teamname, $teamplayername, $role, $stamp));

						# FIXME: check duplicates, let db do it?
					else :
						$inputerror = 1;
					endif;
				else :
					$autherror = 1;
				endif;
			elseif ($method == "PUT") :
				if ($authenticated) :
					if (($xmlroot) && ($xmlroot->tagName == "teamplayer")) :
						$nodes = $xmlroot->getElementsByTagName("*");
						foreach ($nodes as $node)
						{
							if ($node->nodeName == "role") :
								$role = $node->nodeValue;
							endif;
						}

						db_exec($conn,
							"UPDATE teammembers SET role = '%^' " .
							"WHERE teamname = '%^' AND handle = '%^'",
							array($role, $teamname, $teamplayername));

						# FIXME: check duplicates, let db do it?
					else :
						$inputerror = 1;
					endif;
				else :
					$autherror = 1;
				endif;
			elseif ($method == "DELETE") :
				if ($authenticated) :
					db_exec($conn,
						"DELETE FROM teammembers WHERE teamname = '%^' AND handle = '%^'",
						array($teamname, $teamplayername));
					# FIXME: check presence etc.
				else :
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

if ($autherror) :
	$error = 1;
endif;
if ($internalerror) :
	$error = 1;
endif;
if ($inputerror) :
	$error = 1;
endif;

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
elseif ($inputerror) :
	header("Content-type: text/plain");
	header("HTTP/1.1 415 It's make or break");

	echo "Error: invalid input data";
elseif ($internalerror) :
	header("Content-type: text/plain");
	header("HTTP/1.1 501 System suffers badly");

	echo "Error: internal API error";
else :
	header("Content-type: text/plain");
	header("HTTP/1.1 501 There was an error");

	echo "Error: undefined resource or method ($resource, $method)";
endif;

?>
