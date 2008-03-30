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

		$this->connectlength = 0;
		$this->stacks = array();

		$this->players["player2"] = "AI";
	}

	function load($savegame)
	{
		$f = fopen($savegame, "r");
		while ($a = fgets($f, 1024))
		{
			$a = substr($a, 0, strlen($a) - 1);
			$ar = explode(" ", $a);

			if ($ar[0] == "option") :
				if ($ar[1] == "boardwidth") :
					$this->width = $ar[2];
					$this->history .= "The board width is $this->width.<br>";

					for ($i = 0; $i < $this->width; $i++)
					{
						$this->stacks[$i] = 0;
					}
				elseif ($ar[1] == "boardheight") :
					$this->height = $ar[2];
					$this->history .= "The board height is $this->height.<br>";
				elseif ($ar[1] == "connectlength") :
					$this->connectlength = $ar[2];
					$this->history .= "The connection length is $this->connectlength.<br>";
				endif;
			endif;

			if ($ar[0] == "move") :
				$column = $ar[2] - 1;
				$y = $this->height - $this->stacks[$column] - 1;
				$this->game[$column][$y] = $turn + 1;
				$turn = ($turn + 1) % 2;
				$this->stacks[$column] += 1;
				$player = $this->players[$ar[1]];
				$this->history .= "Player $player sets on column $column.<br>";
			elseif ($ar[0] == "winner") :
				$this->winner = $this->players["player" . ($ar[1] + 1)];
				$this->history .= "The game ends with a winner: $this->winner!<br>";
			endif;

			if (($ar[0] == "player1") || ($ar[0] == "player2")) :
				if ($ar[1] == "joins") :
					$this->players[$ar[0]] = $ar[3];
					$player = $ar[3];
					$seat = substr($ar[0], -1, 1);
					$this->history .= "Player $player joins the game on seat $seat.<br>";
				endif;
			endif;
		}
	}
}

?>
