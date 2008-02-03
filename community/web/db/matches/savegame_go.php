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

	var $winnercolour;
	var $winnerscore;

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
		$sgf = "";
		$f = fopen($savegame, "r");
		while ($a = fgets($f, 1024))
		{
			$a = substr($a, 0, strlen($a) - 1);
			$a = str_replace("\n", "", $a);
			$a = str_replace("\r", "", $a);
			$sgf .= $a;
		}

		$this->parse($sgf, 0);

		if ($this->winnercolour == "B") :
			$this->winner = $this->players[0];
		elseif ($this->winnercolour == "W") :
			$this->winner = $this->players[1];
		endif;
	}

	function parse($data, $offset)
	{
		$incomment = false;
		$buf = "";
		$newoffset = strlen($data);
		$cold = null;

		for ($i = $offset; $i < strlen($data); $i++)
		{
			$c = $data[$i];

			if ($incomment) :
				if ($c == ']') :
					$incomment = false;
				endif;

				continue;
			endif;

			if ($c == '(') :
				#echo "//from $i//\n";
				$i = $this->parse($data, $i + 1);
				#echo "//jumpto $i//\n";
			elseif ($c == ')') :
				$newoffset = $i + 1;
				break;
			elseif ($c == 'C') :
				if (($cold == ';') || ($cold == ']')) :
					$incomment = true;
				endif;
			else :
				$buf .= $c;
			endif;

			$cold = $c;
		}

		#if ($offset == 0) :
		#	echo "((___ $buf ___))\n";
		#else :
		#	echo "<<[$offset:$newoffset] $buf >>\n";
		#endif;

		$stmts = explode(";", $buf);
		foreach ($stmts as $stmt)
		{
			#echo "*** $stmt\n";

			$prop = "";
			$propval = "";
			$inproperty = false;
			$firstpropval = "";

			for ($i = 0; $i < strlen($stmt); $i++)
			{
				$c = $stmt[$i];
				if ($c == '[') :
					$inproperty = true;
				elseif ($c == ']') :
					$inproperty = false;

					#echo "=> $prop = $propval\n";
					$this->evaluate($prop, $propval);

					$prop = "";
					$propval = "";
				else :
					if ($inproperty) :
						$propval .= $c;
					else :
						$prop .= $c;
					endif;
				endif;
			}
		}

		return $newoffset;
	}

	function evaluate($prop, $propval)
	{
		if (($prop == "B") || ($prop == "W")) :
			// black or white move
			$x = ord($propval[0]) - ord('a');
			$y = ord($propval[1]) - ord('a');
			$this->game[$x][$y] = $prop;
		elseif ($prop == "KO") :
			// FIXME: execute even if illegal - but which move?
		elseif ($prop == "MN") :
			// FIXME: sets move number explicitly
		elseif ($prop == "AB") :
			// black stone setup
		elseif ($prop == "AW") :
			// white stone setup
		elseif ($prop == "AE") :
			// clearance
		elseif ($prop == "PL") :
			if ($propval == "B") :
				$this->turn = 0;
			elseif ($propval == "W") :
				$this->turn = 1;
			endif;
		elseif ($prop == "C") :
			// FIXME: assign comment here and don't filter out beforehand
			// (\ is used for escaping)
		elseif ($prop == "DM") :
			// good for both
		elseif ($prop == "GB") :
			// good for black
		elseif ($prop == "GW") :
			// good for white
		elseif ($prop == "UC") :
			// unclear if good for anybody
		elseif ($prop == "HO") :
			// node is hotspot (= interesting)
		elseif ($prop == "N") :
			// name of a node
		elseif ($prop == "V") :
			// value of a node, i.e. estimated score
		elseif ($prop == "BM") :
			// bad move
		elseif ($prop == "DO") :
			// doubtful move
		elseif ($prop == "IT") :
			// interesting move
		elseif ($prop == "TE") :
			// good move («tesuji»)
		elseif ($prop == "AR") :
			// draw arrow (double data!)
		elseif ($prop == "CR") :
			// mark points with circle
		elseif ($prop == "DD") :
			// dim out points
		elseif ($prop == "LB") :
			// label
		elseif ($prop == "LN") :
			// draw a line
		elseif ($prop == "MA") :
			// marks point with an X
		elseif ($prop == "SL") :
			// selects the points, unknown markup
		elseif ($prop == "SQ") :
			// marks points with a square
		elseif ($prop == "TR") :
			// marks points with a triangle
		elseif ($prop == "AP") :
			// application name
		elseif ($prop == "CA") :
			// character set, default is latin-1 :(
		elseif ($prop == "FF") :
			// file format, default is 1, should be 4
		elseif ($prop == "GM") :
			// type of game: 1 = go, >1 = other board games
		elseif ($prop == "ST") :
			// style of how to show variations; bitmask!
		elseif ($prop == "CP") :
			// copyright information
		elseif ($prop == "EV") :
			// name of the event
		elseif ($prop == "GN") :
			// name of the game (match)
		elseif ($prop == "GC") :
			// extra information about the game
		elseif ($prop == "ON") :
			// information on the opening
		elseif ($prop == "OT") :
			// information on the overtime
		elseif ($prop == "PC") :
			// information on where the game was played
		elseif ($prop == "RE") :
			// game result
			if (($propval == "0") || ($propval == "Draw")) :
				$this->winner = null;
			else :
				$ar = explode("+", $propval);
				$this->winnercolour = $ar[0];
				$this->winnerscore = $ar[1];
			endif;
		elseif ($prop == "RO") :
			// Round number
		elseif ($prop == "RU") :
			// Game rules
		elseif ($prop == "SO") :
			// Source of the game
		elseif ($prop == "TM") :
			// Time limit in seconds
		elseif ($prop == "US") :
			// User or program that entered the game
		elseif ($prop == "BT") :
			// name of black team
		elseif ($prop == "WT") :
			// name of white team
		elseif ($prop == "BR") :
			// rank of black player
		elseif ($prop == "WR") :
			// rank of white player
		elseif ($prop == "AN") :
			// name of annotator
		elseif ($prop == "PB") :
			// name of black player
			$this->players[0] = $propval;
		elseif ($prop == "PW") :
			// name of white player
			$this->players[1] = $propval;
		elseif ($prop == "SZ") :
			// board size
			$this->width = $propval;
			$this->height = $propval;
		elseif ($prop == "DT") :
			// date
			$this->starttime = $propval;
		elseif ($prop == "OB") :
			// number of black moves left
		elseif ($prop == "OW") :
			// number of white moves left
		elseif ($prop == "BL") :
			// time left for black after move
		elseif ($prop == "WL") :
			// time left for white after move
		elseif ($prop == "FG") :
			// figure division (for printing) - huh?
		elseif ($prop == "PM") :
			// how numbers are printed
		elseif ($prop == "VW") :
			// view only part of the board
		endif;
	}
}

?>
