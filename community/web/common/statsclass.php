<?php

class Statistics
{
	var $players_number;
	var $teams_number;
	var $matches_number;
	var $tournaments_number;
	var $games_number;
	var $rankings_number;

	var $database;

	function Statistics()
	{
	}

	function setConnection($database)
	{
		$this->database = $database;
	}

	function version()
	{
		$database = $this->database;

		$res = $database->exec("SELECT value FROM control WHERE key = 'version'");
		$version = $database->result($res, 0, "value");
		return $version;
	}

	function calculateGeneral()
	{
		$database = $this->database;

		$res = $database->exec("SELECT COUNT(*) FROM users");
		$this->players_number = $database->result($res, 0, "count");

		$res = $database->exec("SELECT COUNT(*) FROM teams");
		$this->teams_number = $database->result($res, 0, "count");

		$res = $database->exec("SELECT COUNT(*) FROM stats");
		$this->rankings_number = $database->result($res, 0, "count");

		$res = $database->exec("SELECT COUNT(*) FROM tournaments");
		$this->tournaments_number = $database->result($res, 0, "count");

		$res = $database->exec("SELECT COUNT(*) FROM matches");
		$this->matches_number = $database->result($res, 0, "count");

		$res = $database->exec("SELECT COUNT(*) FROM (SELECT DISTINCT game FROM stats) AS count");
		$this->games_number = $database->result($res, 0, "count");
	}

	function listTeams($max = 999, $random = false)
	{
		$database = $this->database;

		if ($random) :
			$sort = "RANDOM()";
		else :
			$sort = "teamname ASC";
		endif;

		$res = $database->exec("SELECT teamname FROM teams ORDER BY $sort LIMIT $max");
		for ($i = 0; $i < $database->numrows($res); $i++)
		{
			$team = $database->result($res, $i, "teamname");

			if ($i == $max - 1) :
				echo "\t<a href=\"/db/teams/\" class=\"menuitem\" title=\"More...\">More...</a>\n";
			else:
				echo "\t<a href=\"/db/teams/?lookup=$team\" class=\"menuitem\" title=\"$team\">$team</a>\n";
			endif;
		}
	}

	function listPlayers($max = 999, $random = false)
	{
		$database = $this->database;

		if ($random) :
			$sort = "RANDOM()";
		else :
			$sort = "handle ASC";
		endif;

		$res = $database->exec("SELECT handle FROM users ORDER BY $sort LIMIT $max");
		for ($i = 0; $i < $database->numrows($res); $i++)
		{
			$handle = $database->result($res, $i, "handle");

			if ($i == $max - 1) :
				echo "\t<a href=\"/db/players/\" class=\"menuitem\" title=\"More...\">More...</a>\n";
			else:
				echo "\t<a href=\"/db/players/?lookup=$handle\" class=\"menuitem\" title=\"$handle\">$handle</a>\n";
			endif;
		}
	}

	function listGames($max = 999, $random = false)
	{
		$database = $this->database;

		if ($random) :
			$sort = "RANDOM()";
		else :
			$sort = "game ASC";
		endif;

		$res = $database->exec("SELECT game FROM (SELECT DISTINCT game FROM stats) " .
			"AS game ORDER BY $sort LIMIT $max");
		for ($i = 0; $i < $database->numrows($res); $i++)
		{
			$game = $database->result($res, $i, "game");

			if ($i == $max - 1) :
				echo "\t<a href=\"/db/games/\" class=\"menuitem\" title=\"More...\">More...</a>\n";
			else:
				echo "\t<a href=\"/db/games/?lookup=$game\" class=\"menuitem\" title=\"$game\">$game</a>\n";
			endif;
		}
	}

	function listMatches($max = 999)
	{
		$database = $this->database;

		$res = $database->exec("SELECT game, winner, date, id FROM matches ORDER BY date DESC LIMIT $max");
		for ($i = 0; $i < $database->numrows($res); $i++)
		{
			$game = $database->result($res, $i, "game");
			$winner = $database->result($res, $i, "winner");
			$tid = $database->result($res, $i, "id");

			echo "\t<a href=\"/db/matches/?lookup=$tid\" class=\"menuitem\" title=\"$game\">$game ($winner)</a>\n";
		}
	}

	function listTournaments($max = 999)
	{
		$database = $this->database;

		$res = $database->exec("SELECT game, name, date, id FROM tournaments ORDER BY date DESC LIMIT $max");
		for ($i = 0; $i < $database->numrows($res); $i++)
		{
			$game = $database->result($res, $i, "game");
			$name = $database->result($res, $i, "name");
			$tid = $database->result($res, $i, "id");

			echo "\t<a href=\"/db/tournaments/?lookup=$tid\" class=\"menuitem\" title=\"$game\">$name ($game)</a>\n";
		}
	}
}

$stat = new Statistics();
$stat->setConnection($database);

?>

