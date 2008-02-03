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
	}

	function parse($data, $offset)
	{
		$incomment = false;
		$buf = "";
		$newoffset = strlen($data);

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
				$incomment = true;
			else :
				$buf .= $c;
			endif;
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
			$x = ord($propval[0]) - ord('A');
			$y = ord($propval[1]) - ord('a');
			$this->game[$x][$y] = $prop;
		elseif ($prop == "SZ") :
			$this->width = $propval;
			$this->height = $propval;
		elseif ($prop == "DT") :
			$this->starttime = $propval;
		endif;
	}
}

?>
