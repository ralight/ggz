<?php

include_once("database.php");

class Match
{
	var $id;
	var $game;
	var $date;
	var $winner;
	var $playertypes;

	function Match($lookup)
	{
		global $database;

		$this->playertypes = array();
		$this->id = $lookup;

		$this->load();
	}

	function load()
	{
		global $database;

		$res = $database->exec("SELECT * FROM matches WHERE id = '$this->id'");
		if ($database->numrows($res) == 1) :
			$this->game = $database->result($res, 0, "game");
			$this->winner = $database->result($res, 0, "winner");
			$date = $database->result($res, 0, "date");
			$this->date = date("d.m.Y", $date);

			$res = $database->exec("SELECT * FROM matchplayers WHERE match = '$this->id'");
			for ($i = 0; $i < $database->numrows($res); $i++)
			{
				$handle = $database->result($res, $i, "handle");
				$playertype = $database->result($res, $i, "playertype");

				$this->playertypes[$handle] = $playertype;
			}
		endif;
	}

	function link($handle)
	{
		$playertype = $this->playertypes[$handle];
		if ($playertype == "registered") :
			$link = "<a href='/db/players/?lookup=$handle'>$handle ($playertype)</a>";
		else :
			$link = "<span class='linklike'>$handle ($playertype)</span>";
		endif;
		return $link;
	}
}

?>

