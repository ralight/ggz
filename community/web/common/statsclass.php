<?php

class Statistics
{
	var $players_number;
	var $teams_number;
	var $matches_number;
	var $tournaments_number;
	var $games_number;
	var $rankings_number;

	var $id;

	function Statistics()
	{
	}

	function setConnection($id)
	{
		$this->id = $id;
	}

	function version()
	{
		$res = pg_exec($this->id, "SELECT value FROM control WHERE key = 'version'");
		$version = pg_result($res, 0, "value");
		return $version;
	}

	function calculateGeneral()
	{
		$res = pg_exec($this->id, "SELECT COUNT(*) FROM users");
		$this->players_number = pg_result($res, 0, "count");

		$res = pg_exec($this->id, "SELECT COUNT(*) FROM teams");
		$this->teams_number = pg_result($res, 0, "count");

		$res = pg_exec($this->id, "SELECT COUNT(*) FROM stats");
		$this->rankings_number = pg_result($res, 0, "count");

		$res = pg_exec($this->id, "SELECT COUNT(*) FROM tournaments");
		$this->tournaments_number = pg_result($res, 0, "count");

		$res = pg_exec($this->id, "SELECT COUNT(*) FROM matches");
		$this->matches_number = pg_result($res, 0, "count");

		$res = pg_exec($this->id, "SELECT COUNT(*) FROM (SELECT DISTINCT game FROM stats) AS count");
		$this->games_number = pg_result($res, 0, "count");
	}

	function listTeams($max = 999)
	{
		$res = pg_exec($this->id, "SELECT teamname FROM teams ORDER BY teamname LIMIT $max");
		for ($i = 0; $i < pg_numrows($res); $i++)
		{
			$team = pg_result($res, $i, "teamname");

			if ($i == $max - 1) :
				echo "\t<a href=\"/db/teams/\" class=\"menuitem\" title=\"More...\">More...</a>\n";
			else:
				echo "\t<a href=\"/db/teams/?lookup=$team\" class=\"menuitem\" title=\"$team\">$team</a>\n";
			endif;
		}
	}

	function listPlayers($max = 999)
	{
		$res = pg_exec($this->id, "SELECT handle FROM users ORDER BY handle ASC LIMIT $max");
		for ($i = 0; $i < pg_numrows($res); $i++)
		{
			$handle = pg_result($res, $i, "handle");

			if ($i == $max - 1) :
				echo "\t<a href=\"/db/players/\" class=\"menuitem\" title=\"More...\">More...</a>\n";
			else:
				echo "\t<a href=\"/db/players/?lookup=$handle\" class=\"menuitem\" title=\"$handle\">$handle</a>\n";
			endif;
		}
	}

	function listGames($max = 999)
	{
		$res = pg_exec($this->id, "SELECT DISTINCT game FROM stats ORDER BY game ASC LIMIT $max");
		for ($i = 0; $i < pg_numrows($res); $i++)
		{
			$game = pg_result($res, $i, "game");

			if ($i == $max - 1) :
				echo "\t<a href=\"/db/games/\" class=\"menuitem\" title=\"More...\">More...</a>\n";
			else:
				echo "\t<a href=\"/db/games/?lookup=$game\" class=\"menuitem\" title=\"$game\">$game</a>\n";
			endif;
		}
	}

	function listMatches($max = 999)
	{
		$res = pg_exec($this->id, "SELECT game, winner, date, id FROM matches ORDER BY date DESC LIMIT $max");
		for ($i = 0; $i < pg_numrows($res); $i++)
		{
			$game = pg_result($res, $i, "game");
			$winner = pg_result($res, $i, "winner");
			$tid = pg_result($res, $i, "id");

			echo "\t<a href=\"/db/matches/?lookup=$tid\" class=\"menuitem\" title=\"$game\">$game ($winner)</a>\n";
		}
	}

	function listTournaments($max = 999)
	{
		$res = pg_exec($this->id, "SELECT game, name, date, id FROM tournaments ORDER BY date DESC LIMIT $max");
		for ($i = 0; $i < pg_numrows($res); $i++)
		{
			$game = pg_result($res, $i, "game");
			$name = pg_result($res, $i, "name");
			$tid = pg_result($res, $i, "id");

			echo "\t<a href=\"/db/tournaments/?lookup=$tid\" class=\"menuitem\" title=\"$game\">$name ($game)</a>\n";
		}
	}
}

$stat = new Statistics();
$stat->setConnection($id);

?>

