<?php

class Savegame
{
	var $game;
	var $width;
	var $height;
	var $turn;
	var $starttime;
	var $endtime;
	var $winner;

	function Savegame()
	{
		$this->game = array();
		$this->width = 0;
		$this->height = 0;
		$this->turn = 0;
		$this->winner = "";
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
				elseif ($ar[1] == "start") :
					$this->starttime = $ar[2];
				elseif ($ar[1] == "tie") :
					$this->winner = "";
					$this->endtime = $ar[2];
				endif;
			endif;
			if (($ar[0] == "player0") || ($ar[0] == "player1")) :
				if ($ar[1] == "move") :
					$x = $ar[2];
					$y = $ar[3];
					$this->game[$x][$y] = $turn + 1;
					$turn = ($turn + 1) % 2;
				elseif ($ar[1] == "winner") :
					$this->winner = $ar[0];
					$this->endtime = $ar[2];
				endif;
			endif;
		}
	}
}

?>
