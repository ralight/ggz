<?php

class Savegame
{
	var $game;
	var $width;
	var $height;
	var $starttime;
	var $endtime;
	var $winner;
	var $history;

	var $turn;
	var $players;

	function Savegame()
	{
		$this->game = array();
		$this->width = 0;
		$this->height = 0;
		$this->turn = 0;
		$this->winner = "";
		$this->players = array();
		$this->history = "";
	}

	function load($savegame)
	{
		$f = fopen($savegame, "r");
		while ($a = fgets($f, 1024))
		{
			$a = substr($a, 0, strlen($a) - 1);

			$ar = explode(" ", $a);
			if ($ar[0] == "players") :
				if ($ar[1] == "options") :
					$this->width = $ar[2];
					$this->height = $ar[3];
					$this->history .= "The board size is determined to be $this->width x $this->height.<br>";
				elseif ($ar[1] == "start") :
					$this->starttime = $ar[2];
					$timestr = date("H:i:s", $this->starttime);
					$this->history .= "The game is started at $timestr.<br>";
				elseif ($ar[1] == "continue") :
					$continuetime = $ar[2];
					$timestr = date("H:i:s", $continuetime);
					$this->history .= "The game is interrupted!<br>";
					$this->history .= "The game is continued at $timestr.<br>";
				elseif ($ar[1] == "tie") :
					$this->winner = "";
					$this->endtime = $ar[2];
					$this->history .= "The game ends with a tie!<br>";
				endif;
			endif;
			if (($ar[0] == "player1") || ($ar[0] == "player2")) :
				if ($ar[1] == "move") :
					$x = $ar[2] + 1;
					$y = $ar[3] + 1;
					$this->game[$x - 1][$y - 1] = $turn + 1;
					$turn = ($turn + 1) % 2;
					$player = $this->players[$ar[0]];
					$this->history .= "Player $player sets on $x/$y.<br>";
				elseif ($ar[1] == "winner") :
					$this->winner = $this->players[$ar[0]];
					$this->endtime = $ar[2];
					$winner = $this->winner;
					$this->history .= "The game ends with a winner: $winner!<br>";
				elseif ($ar[1] == "join") :
					$this->players[$ar[0]] = $ar[2];
					$player = $ar[2];
					$seat = substr($ar[0], -1, 1);
					$this->history .= "Player $player joins the game on seat $seat.<br>";
				elseif ($ar[1] == "leave") :
					$player = $ar[2];
					$this->history .= "Player $player leaves the game.<br>";
				endif;
			endif;
		}
	}
}

?>
