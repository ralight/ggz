<?php

global $community_locale_lang;

class Locale
{
	function Locale()
	{
	}

	function detect()
	{
		global $community_locale_lang;

		$f = $_SERVER["SCRIPT_FILENAME"];
		$far = explode(".", $f);
		$ending = $far[sizeof($far) - 1];
		if ($ending != "php") :
			$community_locale_lang = $ending;
		endif;
	}

	function includefile($file)
	{
		global $community_locale_lang;

		$lang = $community_locale_lang;
		$ret = @include("$file.$lang");
		if(!$ret) :
			include($file);
		endif;
	}

	function languagename($s)
	{
		if ($s == "de") return "Deutsch";
		if ($s == "en") return "English";
		if ($s == "sv") return "Svenska";
		if ($s == "fr") return "Français";
		return "($s)";
	}

	function availablelanguages()
	{
		global $community_locale_lang;

		$dar = explode("/", $_SERVER['SCRIPT_FILENAME']);
		$i = 1;
		while($dar[$i]) $i++;
		$file = $dar[$i - 1];

		$i = strlen($file);
		while($file[$i] != ".") $i--;
		$file = substr($file, 0, $i);

		$dir = $_SERVER['SCRIPT_FILENAME'];
		$i = strlen($dir);
		while($dir[$i] != "/") $i--;
		$dir = substr($dir, 0, $i);

		echo "Available languages: ";

		$d = opendir($dir);
		while($f = readdir($d))
		{
			if(substr($f, strlen($f) - 4, 4) == ".php") continue;
			if(substr($f, strlen($f) - 8, 8) == ".leftbar") continue;
			if(substr($f, strlen($f) - 9, 9) == ".rightbar") continue;

			if(substr($f, 0, strlen("$file.")) == "$file.") :
				$far = explode(".", $f);
				$i = 0;
				while($far[$i]) $i++;
				$lang = $far[$i - 1];
				$language = Locale::languagename($lang);
				if ($lang != $community_locale_lang) :
					if (!strstr($_SERVER['PHP_SELF'], $_SERVER['SCRIPT_FILENAME'])) :
						$f = "../$f";
					endif;
					echo "<a href='$f'>$language</a> ";
				else :
					echo "$language ";
				endif;
			endif;
		}
		closedir($d);
	}
}

?>
