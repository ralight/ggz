<?php

function country($lang)
{
	$langs = $_SERVER['HTTP_ACCEPT_LANGUAGE'];
	$langs = str_replace(" ", "", $langs);
	$langs = preg_replace("/;[^,]*/", "", $langs);
	$ar = split(",", $langs);

	foreach ($ar as $l)
	{
		if($lang == $l) break;
		if(($lang == "en") && ($l == "en-us")) $lang = "en-us";
		if(($lang == "en") && ($l == "en-ca")) $lang = "en-ca";
		if(($lang == "en") && ($l == "en-gb")) $lang = "en-gb";
		if(($lang == "pt") && ($l == "pt-br")) $lang = "pt-br";
		if(($lang == "de") && ($l == "de-ch")) $lang = "de-ch";
		if(($lang == "de") && ($l == "de-at")) $lang = "de-at";
	}

	switch($lang)
	{
		case "en": return "us"; break;
		case "en-gb": return "gb"; break;
		case "en-us": return "us"; break;
		case "en-ca": return "ca"; break;
		case "pt-br": return "br"; break;
		case "de-ch": return "ch"; break;
		case "de-at": return "at"; break;
		default: return $lang;
	}
}

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

$d = opendir($dir);
while($f = readdir($d))
{
	if(substr($f, strlen($f) - 4, 4) == ".php") continue;
	if(substr($f, 0, strlen("$file.")) == "$file.") :
		$far = explode(".", $f);
		$i = 0;
		while($far[$i]) $i++;
		$lang = $far[$i - 1];
		$country = country($lang);

		$img = "/pics/site/flags/flag-$country.png";
		echo "<a href=\"$file.$lang\"><img class=\"flag\" src=\"$img\" alt=\"$lang\"></a>\n";
	endif;
}
closedir($d);
?>
