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
		$this->width = 8;
		$this->height = 8;

		$header = 1;
		$moves = "";

		$f = fopen($savegame, "r");
		while ($a = fgets($f, 1024))
		{
			$a = substr($a, 0, strlen($a) - 1);
			$a = str_replace("\n", "", $a);
			$a = str_replace("\r", "", $a);

			if(preg_match("/^\[(\S+)\s\"(.*)\"\]$/", $a, $matches)) :
				$key = $matches[1];
				$value = $matches[2];

				if ($key == "Date") :
					preg_match("/^(\d+)\.(\d+)\.(\d+)$/", $value, $matches);
					$year = $matches[1];
					$month = $matches[2];
					$day = $matches[3];
					$stamp = mktime(0, 0, 0, $month, $day, $year);
					$this->starttime = $stamp;
					$this->endtime = $stamp;
				elseif ($key == "White") :
					$this->players[0] = $value;
				elseif ($key == "Black") :
					$this->players[1] = $value;
				elseif ($key == "Result") :
					$ar = explode("-", $value);
					if ($ar[0] == "0") :
						$this->winner = $this->players[1];
					elseif ($ar[1] == "0") :
						$this->winner = $this->players[0];
					endif;
				endif;

				$header = 1;
			elseif ($a == "") :
				if (!$header) :
					$ar = explode(" ", $moves);
					for ($i = 0; $i < sizeof($ar); $i += 3)
					{
						$num = $ar[$i];
						$white = $ar[$i + 1];
						$black = $ar[$i + 2];
						$this->history .= "Move $num $white, $black<br>\n";
						foreach (array($white, $black) as $move)
						{
							$figure = $move[0];
							$xmove = substr($move, 1, strlen($move) - 1);

							if ($xmove[0] == "x") :
								$xmove = substr($xmove, 1, strlen($xmove) - 1);
							endif;
							if ($xmove[strlen($xmove) - 1] == "+") :
								$xmove = substr($xmove, 0, strlen($xmove) - 1);
							endif;

							if ($figure == "K") :
								$movestr = "King $xmove";
							elseif ($figure == "N") :
								$movestr = "Knight $xmove";
							elseif ($figure == "B") :
								$mmovestr = "Bishop $xmove";
							elseif ($figure == "R") :
								$movestr = "Rook $xmove";
							elseif ($figure == "Q") :
								$movestr = "Queen $xmove";
							elseif (strlen($move) == 2) :
								$xmove = "$figure$xmove";
								$movestr = "Pawn $xmove";
								$figure = "P";
							elseif (strlen($move) == 4) :
								$movestr = "Pawn (capturing) $xmove";
								$figure = "P";
							else :
								//echo "UNHANDLED: $move<br>";
								continue;
							endif;

							if (strlen($xmove) == 3) :
								$xmove = substr($xmove, 1, strlen($xmove) - 1);
							endif;

							$colour = "white";
							if ($move == $black) $colour = "black";

							//$this->history .= "($colour: $movestr)<br>\n";

							$x = ord($xmove[0]) - ord("a");
							$y = $xmove[1];
							$this->game[$x][$y] = "$colour:$figure";
						}
						# O-O-O, O-O: Rochade
						# dxe5: Schlagzug
						# a8: links oben, h1: rechts unten
					}
					$moves = "";
				endif;
				$header = 1 - $header;
			else :
				if (!$header) :
					if ($moves) $moves .= " ";
					$moves .= $a;
				endif;
			endif;
		}
	}
}

?>
