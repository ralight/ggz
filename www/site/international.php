<?php

function country($lang)
{
	switch($lang)
	{
		case "en": return "gb"; break;
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
	if(strstr($f, "$file.")) :
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
