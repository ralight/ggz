<?php

//require_once(".htconf");
//$id = pg_connect("host=$dbhost dbname=$dbname user=$dbuser password=$dbpass");

include_once("database.php");
include_once("auth.php");

if (!Auth::username()) :
	exit;
else:
	$ggzuser = Auth::username();
endif;

$team_name = $_POST["team_name"];
$team_full = $_POST["team_full"];
$team_homepage = $_POST["team_homepage"];
$team_logo = $_POST["team_logo"];
$player_name = $_POST["player_name"];
$player_approval = $_POST["player_approval"];
$player_role = $_POST["player_role"];

if ($join == 1) :
	$res = pg_exec($id, "SELECT * FROM teammembers WHERE teamname = '$team_name' AND handle = '$ggzuser'");
	if (($res) && (pg_numrows($res) == 0)) :
		$res = pg_exec($id, "INSERT INTO teammembers " .
			"(teamname, handle, role) VALUES " .
			"('$team_name', '$ggzuser', '')");
	else:
		pg_exec($id, "DELETE FROM teammembers WHERE teamname = '$team_name' AND handle = '$ggzuser'");
	endif;
elseif ($create == 1) :
	$res = pg_exec($id, "SELECT * FROM teams WHERE teamname = '$team_name'");
	if (($res) && (pg_numrows($res) == 0)) :
		$stamp = time();
		$res = pg_exec($id, "INSERT INTO teams " .
			"(teamname, fullname, icon, foundingdate, homepage, founder) VALUES " .
			"('$team_name', '$team_full', '$team_logo', $stamp, '$team_homepage', '$ggzuser')");
		$res = pg_exec($id, "INSERT INTO teammembers " .
			"(teamname, handle, role) VALUES " .
			"('$team_name', '$ggzuser', 'leader,founder,member')");
	endif;
elseif ($approve == 1) :
	if ($player_approval == 'approved') :
		$res = pg_exec($id, "UPDATE teammembers SET " .
			"role = 'member' " .
			"WHERE teamname = '$team_name' AND handle = '$player_name' AND role = ''");
	else :
		$res = pg_exec($id, "DELETE FROM teammembers " .
			"WHERE teamname = '$team_name' AND handle = '$player_name' AND role = ''");
	endif;
elseif ($manage == 1) :
	$res = pg_exec($id, "SELECT * FROM teammembers WHERE teamname = '$team_name' AND handle = '$player_name'");
	if (($res) && (pg_numrows($res) == 1)) :
		$role = pg_result($res, 0, "role");
		if ($player_role != "member") :
			$player_role .= ",member";
		endif;
		if (strstr($role, "founder")) :
			$player_role .= ",founder";
		endif;
		$res = pg_exec($id, "UPDATE teammembers SET " .
			"role = '$player_role' " .
			"WHERE teamname = '$team_name' AND handle = '$player_name'");
	endif;
endif;

header("Location: index.php");

?>

