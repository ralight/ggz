<?php

require_once(".htconf");

$id = pg_connect("host=$dbhost dbname=$dbname user=$dbuser password=$dbpass");

if (($id) && ($ggzuser)) :
	$res = pg_exec($id, "SELECT * FROM users WHERE password = '$ggzuser'");
	if (($res) && (pg_numrows($res) == 1)) :
		$ggzuser = pg_result($res, 0, "handle");
	else :
		unset($ggzuser);
	endif;
endif;

$team_name = $_POST["team_name"];
$team_full = $_POST["team_full"];
$team_homepage = $_POST["team_homepage"];
$player_name = $_POST["player_name"];
$player_approval = $_POST["player_approval"];
$player_role = $_POST["player_role"];

if ($ggzuser) :
	if ($join == 1) :
		$res = pg_exec($id, "SELECT * FROM teammembers WHERE teamname = '$team_name' AND handle = '$ggzuser'");
		if (($res) && (pg_numrows($res) == 0)) :
			$res = pg_exec($id, "INSERT INTO teammembers " .
				"(teamname, handle, role) VALUES " .
				"('$team_name', '$ggzuser', '')");
		endif;
	elseif ($create == 1) :
		$res = pg_exec($id, "SELECT * FROM teams WHERE teamname = '$team_name'");
		if (($res) && (pg_numrows($res) == 0)) :
			$stamp = time();
			$res = pg_exec($id, "INSERT INTO teams " .
				"(teamname, fullname, icon, foundingdate, homepage) VALUES " .
				"('$team_name', '$team_full', '', $stamp, '$team_homepage')");
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
endif;

header("Location: index.php");

?>

