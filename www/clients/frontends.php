<?php

$uri = $_SERVER['REQUEST_URI'];

$uri = preg_replace("/\/clients\//", "", $uri);
$uri = str_replace("/", "", $uri);

$origuri = $uri;
switch($uri)
{
	case "kggz": $uri = "kde"; break;
	case "ggz-gnome": $uri = "gnome"; break;
	case "ggz-gtk": $uri = "gtk"; break;
	case "ggz-txt": $uri = "console"; break;
	case "telggz": $uri = "telnet"; break;
	case "wggz": $uri = "windows"; break;
}

if ($origuri != $uri) :
	header("Location: /clients/$uri/");
else:
	include($_SERVER['DOCUMENT_ROOT'] . "/site/error.php.en");
endif;

?>
