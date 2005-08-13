<?php

class Savegame
{
	var $game;
	var $owners;
	var $width;
	var $height;
	var $winner;
	var $history;

	var $players;

	function Savegame()
	{
		$this->game = array();
		$this->owners = array();
		$this->width = 0;
		$this->height = 0;
		$this->winner = "";
		$this->players = array();
		$this->history = "";
	}

	function load($savegame)
	{
		$this->players["player1"] = "AI";
		$this->players["player2"] = "AI";

		$f = fopen($savegame, "r");
		while ($a = fgets($f, 1024))
		{
			$a = substr($a, 0, strlen($a) - 1);

			$ar = explode(" ", $a);
			if ($ar[0] == "width") :
				$this->width = $ar[1];
			elseif ($ar[0] == "height") :
				$this->height = $ar[1];
				$this->history .= "The board size is determined to be $this->width x $this->height.<br>";
			elseif ($ar[0] == "tie") :
				$this->winner = "";
				$this->history .= "The game ends with a tie!<br>";
			endif;
			if (($ar[0] == "player1") || ($ar[0] == "player2")) :
				if ($ar[1] == "move") :
					$x = $ar[2] + 1;
					$y = $ar[3] + 1;
					$dir = $ar[4];
					if ($this->game[$x - 1][$y - 1]) :
						$this->game[$x - 1][$y - 1] = "both";
					else :
						$this->game[$x - 1][$y - 1] = $dir;
					endif;
					$player = $this->players[$ar[0]];
					$this->history .= "Player $player sets on $x/$y ($dir).<br>";
					if ($dir == "vertical") :
						// to the left
						$c1 = $this->game[$x - 2][$y - 1];
						$c2 = $this->game[$x - 2][$y - 0];
						if (($c1 == "both") && (($c2 == "horizontal") || ($c2 == "both"))) :
							$this->owners[$x - 2][$y - 1] = $ar[0];
						endif;
						// to the right
						$c1 = $this->game[$x - 1][$y - 1];
						$c2 = $this->game[$x - 0][$y - 1];
						$c3 = $this->game[$x - 1][$y - 0];
						if (($c1 == "both") && (($c2 == "vertical") || ($c2 == "both"))) :
							if (($c3 == "horizontal") || ($c3 == "both")) :
								$this->owners[$x - 1][$y - 1] = $ar[0];
							endif;
						endif;
					endif;
					if ($dir == "horizontal") :
						// to the top
						$c1 = $this->game[$x - 1][$y - 2];
						$c2 = $this->game[$x - 0][$y - 2];
						if (($c1 == "both") && (($c2 == "vertical") || ($c2 == "both"))) :
							$this->owners[$x - 1][$y - 2] = $ar[0];
						endif;
						// to the bottom
						$c1 = $this->game[$x - 1][$y - 1];
						$c2 = $this->game[$x - 0][$y - 1];
						$c3 = $this->game[$x - 1][$y - 0];
						if (($c1 == "both") && (($c2 == "vertical") || ($c2 == "both"))) :
							if (($c3 == "horizontal") || ($c3 == "both")) :
								$this->owners[$x - 1][$y - 1] = $ar[0];
							endif;
						endif;
					endif;
				elseif ($ar[1] == "winner") :
					$this->winner = $this->players[$ar[0]];
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
