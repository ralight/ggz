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
}

?>
