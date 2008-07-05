<?php

class Country
{
	var $flagdir;
	var $countrycode;

	function Country()
	{
		$docroot = $_SERVER["DOCUMENT_ROOT"];
		$this->flagdir = "$docroot/db/ggzicons/flags";
	}

	function available()
	{
		if(file_exists($this->flagdir)) :
			return true;
		endif;
		return false;
	}

	function load($code)
	{
		$this->countrycode = $code;
	}

	function name($code)
	{
			$countryname = $code;
			$f = @fopen("$this->flagdir/$code/entry.desktop", "r");
			if ($f) :
				while($a = fgets($f, 1024))
				{
					$ar = explode("=", $a);
					if ($ar[0] == "Name") :
						$countryname = $ar[1];
						$countryname = substr($countryname, 0, strlen($countryname) - 1);
						break;
					endif;
				}
				fclose($f);
			endif;
			return $countryname;
	}

	function flag($code)
	{
		$flag = "$this->flagdir/$code/flag.png";
		if (file_exists($flag)) :
			return "<img src='/db/ggzicons/flags/$code/flag.png'>";
		endif;
		return "";
	}

	function listall()
	{
		$selected = "";
		if (!$this->country) :
			$selected = " SELECTED";
		endif;
		echo "<option value=''$selected>(undisclosed)</option>\n";

		$countrynames = array();
		$countrylist = array();
		$d = opendir($this->flagdir);
		while (($e = readdir($d)))
		{
			if ((is_dir("$this->flagdir/$e")) && ($e[0] != ".")) :
				$countryname = $this->name($e);
				if ($countryname) :
					$countrynames[$countryname] = $e;
					$countrylist[] = $countryname;
				endif;
			endif;
		}
		closedir($d);

		array_multisort($countrylist);

		foreach ($countrylist as $countryname)
		{
			$e = $countrynames[$countryname];
			$selected = "";
			if ($e == $this->countrycode) :
				$selected = " SELECTED";
			endif;
			echo "<option value='$e'$selected>$countryname</option>\n";
		}
	}	
}

?>
