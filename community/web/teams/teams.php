<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

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
	$team_exists = 0;
	$res = $database->exec("SELECT * FROM teams WHERE teamname = '%^'", array($team_name));
	if (($res) && ($database->numrows($res) == 1)) :
		$team_exists = 1;
	endif;
	$res = $database->exec("SELECT * FROM teammembers WHERE teamname = '%^' AND handle = '%^'", array($team_name, $ggzuser));
	if (($res) && ($database->numrows($res) == 0) && ($team_exists)) :
		$res = $database->exec("INSERT INTO teammembers " .
			"(teamname, handle, role) VALUES " .
			"('%^', '%^', '')", array($team_name, $ggzuser));
	else:
		$database->exec("DELETE FROM teammembers WHERE teamname = '%^' AND handle = '%^'", array($team_name, $ggzuser));
	endif;
elseif ($create == 1) :
	$res = $database->exec("SELECT * FROM teams WHERE teamname = '%^'", array($team_name));
	if (($res) && ($database->numrows($res) == 0)) :
		$stamp = time();
		$res = $database->exec("INSERT INTO teams " .
			"(teamname, fullname, icon, foundingdate, homepage, founder) VALUES " .
			"('%^', '%^', '%^', '%^', '%^', '%^')", array($team_name, $team_full, $team_logo, $stamp, $team_homepage, $ggzuser));
		$res = $database->exec("INSERT INTO teammembers " .
			"(teamname, handle, role, entrydate) VALUES " .
			"('%^', '%^', 'leader,founder,member', '%^')", array($team_name, $ggzuser, $stamp));
	endif;
elseif ($approve == 1) :
	if ($player_approval == 'approved') :
		$res = $database->exec("UPDATE teammembers SET " .
			"role = 'member' " .
			"WHERE teamname = '%^' AND handle = '%^' AND role = ''", array($team_name, $player_name));
	else :
		$res = $database->exec("DELETE FROM teammembers " .
			"WHERE teamname = '%^' AND handle = '%^' AND role = ''", array($team_name, $player_name));
	endif;
elseif ($manage == 1) :
	$res = $database->exec("SELECT * FROM teammembers WHERE teamname = '%^' AND handle = '%^'", array($team_name, $player_name));
	if (($res) && ($database->numrows($res) == 1)) :
		$role = $database->result($res, 0, "role");
		if ($player_role != "member") :
			$player_role .= ",member";
		endif;
		if (strstr($role, "founder")) :
			$player_role .= ",founder";
		endif;
		$res = $database->exec("UPDATE teammembers SET " .
			"role = '%^' " .
			"WHERE teamname = '%^' AND handle = '%^'", array($player_role, $team_name, $player_name));
	endif;
endif;

header("Location: index.php");

?>

