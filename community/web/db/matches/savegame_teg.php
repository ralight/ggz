<?php

class Savegame
{
	var $game;

	function Savegame()
	{
		$this->game = array();
	}

	function load($savegame)
	{
		$f = fopen($savegame, "r");
		while ($a = fgets($f, 1024))
		{
			$a = substr($a, 0, strlen($a) - 1);
			if (($a == "") || ($a[0] == "#")) continue;

			$ar = explode(",", $a);
			$entry = array();
			$entry['country'] = $ar[0];
			$entry['player'] = $ar[1];
			$entry['armies'] = $ar[2];

			$this->game[] = $entry;
		}
	}
}

?>
