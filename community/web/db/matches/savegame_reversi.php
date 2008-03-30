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

			$this->width = 8;
			$this->height = 8;

			$ar = explode(" ", $a);
			if ($ar[0] == "white-init") :
				$x = $ar[1];
				$y = $ar[2];
				$this->game[$x - 1][$y - 1] = 1;
				$this->history .= "Preset white on $x/$y.<br>";
			elseif ($ar[0] == "black-init") :
				$x = $ar[1];
				$y = $ar[2];
				$this->game[$x - 1][$y - 1] = 2;
				$this->history .= "Preset black on $x/$y.<br>";
			elseif ($ar[0] == "white") :
				$x = $ar[1];
				$y = $ar[2];
				$this->game[$x - 1][$y - 1] = 1;
				$this->history .= "White on $x/$y.<br>";
			elseif ($ar[0] == "black") :
				$x = $ar[1];
				$y = $ar[2];
				$this->game[$x - 1][$y - 1] = 2;
				$this->history .= "Black on $x/$y.<br>";
			endif;
		}
	}
}

?>
