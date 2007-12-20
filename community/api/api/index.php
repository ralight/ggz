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

// FIXME: this might need folding into common/database.php
function quote($s)
{
	if (!get_magic_quotes_gpc()) :
		return addslashes($s);
	else :
		return $s;
	endif;
}

// -------------------------------------------------------------
// Read the configuration file
// This will give us $ggzcommunitydir, $limitrows, $readonly and $debug

$ret = @include_once(".htconf.php");
if (!$ret) :
	header("Content-type: text/plain");
	header("HTTP/1.1 500 Totally unconfigured");
	echo "Error: File .htconf.php not found";
	exit;
endif;

ini_set("include_path", ini_get("include_path") . ":$ggzcommunitydir/common");

$limit = "";
if ($limitrows > 0) :
	$offset = 0;
	if ($_GET['offset']) :
		$offset = quote($_GET['offset']);
	endif;
	$limit = "LIMIT '$limitrows' OFFSET '$offset'";
endif;

// -------------------------------------------------------------
// Connect to the database

include_once("database.php");

if (!$database) :
	header("Content-type: text/plain");
	header("HTTP/1.1 500 We lost the database");
	echo "Error: Could not connect to the database";
	exit;
endif;

// -------------------------------------------------------------
// So far so good... start working on the input here

$uri = $_SERVER["SCRIPT_NAME"];
$method = $_SERVER["REQUEST_METHOD"];
$resource = str_replace("/api/", "", $uri);

$resourceparts = explode("/", $resource);
$topresource = resourcequote($resourceparts[0]);
$subresource = resourcequote($resourceparts[1]);
$subsubresource = resourcequote($resourceparts[2]);

$error = false;
$autherror = false;
$inputerror = false;
$internalerror = false;

$xmlroot = "";
$authenticated = false;

if (($method == "POST") || ($method == "PUT") || ($method == "DELETE")) :
	if ($readonly) :
		header("Content-type: text/plain");
		header("HTTP/1.1 500 Ain't got no chmod+w here");
		echo "Error: Write access is forbidden at this site";
		exit;
	endif;
endif;

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
		$ret = @$doc->loadXML($input);
		if (!$ret) :
			header("Content-type: text/plain");
			header("HTTP/1.1 500 It's all ggzapi+xml, man");
			echo "Error: Wrongly formatted post contents";
			exit;
		endif;
		$xmlroot = $doc->documentElement;
	endif;
endif;

if (isset($_SERVER["PHP_AUTH_USER"])) :
	$user = $_SERVER["PHP_AUTH_USER"];
	$password = $_SERVER["PHP_AUTH_PW"];

	$res = $database->exec("SELECT * FROM users WHERE handle = '%^' AND password = '%^'",
		array($user, $password));
	if ($database->numrows($res) == 1) :
		$authenticated = true;
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
			$res = $database->exec("SELECT handle FROM users ORDER BY id $limit", null);

			echo "<players>";
			for($i = 0; $i < $database->numrows($res); $i++)
			{
				$playername = $database->result($res, $i, 0);
				echo "<player name='$playername'/>";
			}
			echo "</players>";
		else :
			$error = true;
		endif;
	else :
		$playername = $subresource;
		if ($playername != $user) :
			$authenticated = false;
		endif;

		if ($method == "GET") :
			$res = $database->exec("SELECT email, name FROM users WHERE handle = '%^'",
				array($playername));

			if ($database->numrows($res) == 1) :
				$email = $database->result($res, 0, 0);
				$realname = $database->result($res, 0, 1);

				$res = $database->exec("SELECT photo FROM userinfo WHERE handle = '%^'",
					array($playername));
				if ($database->numrows($res) == 1) :
					$photo = $database->result($res, 0, 0);
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
				$error = true;
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

				$database->exec("INSERT INTO users (handle, password, name, email, firstlogin, permissions) " .
					"VALUES ('%^', '%^', '%^', '%^', %^, %^)",
					array($playername, $password, $realname, $email, $stamp, $perms));

				$database->exec("INSERT INTO userinfo (handle, photo) VALUES ('%^', '%^')",
					array($playername, $photo));
				# FIXME: check duplicates, let db do it?
			else :
				$inputerror = true;
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

					$database->exec("UPDATE users SET password = '%^', email = '%^', name = '%^' " .
						"WHERE handle = '%^'",
						array($password, $email, $realname, $playername));

					$database->exec("UPDATE userinfo SET photo = '%^' WHERE handle = '%^'",
						array($photo, $playername));
					# FIXME: check duplicates, let db do it?
				else :
					$inputerror = true;
				endif;
			else :
				$autherror = true;
			endif;
		elseif ($method == "DELETE") :
			if ($authenticated) :
				$database->exec("DELETE FROM users WHERE handle = '%^'",
					array($playername));
				$database->exec("DELETE FROM userinfo WHERE handle = '%^'",
					array($playername));
				# FIXME: check presence etc.
			else :
				$autherror = true;
			endif;
		else :
			$error = true;
		endif;
	endif;
elseif ($topresource == "statistics") :
	if ($subresource == "games") :
		if ($subsubresource == "") :
			if ($method == "GET") :
				$res = $database->exec("SELECT DISTINCT game FROM stats ORDER BY game $limit", null);

				echo "<games>";
				for($i = 0; $i < $database->numrows($res); $i++)
				{
					$gamename = $database->result($res, $i, 0);
					echo "<game name='$gamename'/>";
				}
				echo "</games>";
			else :
				$error = true;
			endif;
		else :
			if ($method == "GET") :
				$gamename = $subsubresource;
				$res = $database->exec("SELECT handle, rating, highscore FROM stats " .
					"WHERE game = '%^' ORDER BY ranking $limit",
					array($gamename));

				echo "<statistics>";
				for($i = 0; $i < $database->numrows($res); $i++)
				{
					$playername = $database->result($res, $i, 0);
					$rating = $database->result($res, $i, 1);
					$highscore = $database->result($res, $i, 2);
					echo "<statsplayer name='$playername'>";
					echo "<rating>$rating</rating>";
					echo "</statsplayer>";
				}
				echo "</statistics>";
			else :
				$error = true;
			endif;
		endif;
	else :
		$error = true;
	endif;
elseif ($topresource == "teams") :
	if ($subresource == "") :
		if ($method == "GET") :
			$res = $database->exec("SELECT teamname FROM teams $limit", null);

			echo "<teams>";
			for($i = 0; $i < $database->numrows($res); $i++)
			{
				$teamname = $database->result($res, $i, 0);
				echo "<team name='$teamname'/>";
			}
			echo "</teams>";
		else :
			$error = true;
		endif;
	else :
		$teamname = $subresource;
		// FIXME: check against team founder, but only for subsubresource == ""

		if ($subsubresource == "") :
			if ($method == "GET") :
				$res = $database->exec("SELECT founder, foundingdate, fullname, homepage FROM teams " .
					"WHERE teamname = '%^'",
					array($teamname));

				if ($database->numrows($res) == 1) :
					$founder = $database->result($res, 0, 0);
					$foundingdate = $database->result($res, 0, 1);
					$fullname = $database->result($res, 0, 1);
					$homepage = $database->result($res, 0, 1);

					echo "<team name='$teamname'>";
					echo "<founder>$founder</founder>";
					echo "<foundingdate>$foundingdate</foundingdate>";
					echo "<fullname>$fullname</fullname>";
					echo "<homepage>$homepage</homepage>";
					echo "</team>";
				else:
					$error = true;
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

						$database->exec("INSERT INTO teams (teamname, fullname, homepage, founder, foundingdate) " .
							"VALUES ('%^', '%^', '%^', '%^', %^)",
							array($teamname, $fullname, $homepage, $founder, $stamp));

						# FIXME: check duplicates, let db do it?
					else :
						$inputerror = true;
					endif;
				else :
					$autherror = true;
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

						$database->exec("UPDATE teams SET fullname = '%^', homepage = '%^' " .
							"WHERE teamname = '%^'",
							array($fullname, $homepage, $teamname));

						# FIXME: check duplicates, let db do it?
					else :
						$inputerror = true;
					endif;
				else :
					$autherror = true;
				endif;
			elseif ($method == "DELETE") :
				if ($authenticated) :
					$database->exec("DELETE FROM teams WHERE teamname = '%^'",
						array($teamname));
					# FIXME: check presence etc.
				else :
					$autherror = true;
				endif;
			else :
				$error = true;
			endif;
		else :
			$teamplayername = $subsubresource;
			if ($method == "GET") :
				$res = $database->exec("SELECT role, entrydate FROM teammembers " .
					"WHERE teamname = '%^' AND handle = '%^'",
					array($teamname, $teamplayername));

				if ($database->numrows($res) == 1) :
					$role = $database->result($res, 0, 0);
					$entrydate = $database->result($res, 0, 1);

					echo "<teamplayer name='$teamname'>";
					echo "<role>$role</role>";
					echo "<entrydate>$entrydate</entrydate>";
					echo "</teamplayer>";
				else:
					$error = true;
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

						$database->exec("INSERT INTO teammembers (teamname, handle, role, entrydate) " .
							"VALUES ('%^', '%^', '%^', %^)",
							array($teamname, $teamplayername, $role, $stamp));

						# FIXME: check duplicates, let db do it?
					else :
						$inputerror = true;
					endif;
				else :
					$autherror = true;
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

						$database->exec("UPDATE teammembers SET role = '%^' " .
							"WHERE teamname = '%^' AND handle = '%^'",
							array($role, $teamname, $teamplayername));

						# FIXME: check duplicates, let db do it?
					else :
						$inputerror = true;
					endif;
				else :
					$autherror = true;
				endif;
			elseif ($method == "DELETE") :
				if ($authenticated) :
					$database->exec("DELETE FROM teammembers WHERE teamname = '%^' AND handle = '%^'",
						array($teamname, $teamplayername));
					# FIXME: check presence etc.
				else :
					$autherror = true;
				endif;
			else :
				$error = true;
			endif;
		endif;
	endif;
else:
	$error = true;
endif;

// -------------------------------------------------------------
// Finish the script

if ($autherror) :
	$error = true;
endif;
if ($internalerror) :
	$error = true;
endif;
if ($inputerror) :
	$error = true;
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
