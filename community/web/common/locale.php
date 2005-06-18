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

		setlocale(LC_MESSAGES, Locale::localename($community_locale_lang));
		$root = $_SERVER["DOCUMENT_ROOT"];
		bindtextdomain("ggzcommunity", "$root/common/po");
		textdomain("ggzcommunity");
	}

	function language()
	{
		global $community_locale_lang;

		return $community_locale_lang;
	}

	function replacer($matches)
	{
		$s = preg_replace("/^\_\((.*)\)$/", "\$1", $matches[0]);
		$val = _($s);
		$val = preg_replace("/\)\((.*)$/", "", $val);
		return $val;
	}

	function includefile($file)
	{
		global $community_locale_lang;

		$lang = $community_locale_lang;
		ob_start(); 
		$ret = @include("$file.$lang");
		$contents = ob_get_contents(); 
		ob_end_clean(); 
		if(!$ret) :
			ob_start(); 
			include($file);
			$contents = ob_get_contents(); 
			ob_end_clean(); 
		endif;

		$contents = preg_replace_callback("/\_\([^\)]*\)\([^\)]*\)|\_\([^\)]*\)/",
			array("Locale", "replacer"),
			$contents);

		echo $contents;
	}

	function localename($s)
	{
		if ($s == "de") return "de_DE@euro";
		return $s;
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
		$counter = 0;

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
					if (!strstr($_SERVER['SCRIPT_FILENAME'], $_SERVER['PHP_SELF'])) :
						$f = "../$f";
					endif;
					echo "<a href='$f'>$language</a> ";
					$counter += 1;
				else :
					echo "$language ";
				endif;
			endif;
		}
		closedir($d);

		if ($counter == 0) :
			echo Locale::languagename("en");
		endif;
	}
}

?>
