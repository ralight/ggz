<?php

$lang = "en";
$langs = $_SERVER['HTTP_ACCEPT_LANGUAGE'];
$langs = str_replace(" ", "", $langs);
$langs = preg_replace("/;[^,]*/", "", $langs);
$ar = split(",", $langs);

foreach ($ar as $l)
{
	if(file_exists($_SERVER['DOCUMENT_ROOT'] . "/site/header.php.$l")) :
		$lang = $l;
		break;
	endif;

	$lmajor = preg_replace("/-.*/", "", $l);
	if(file_exists($_SERVER['DOCUMENT_ROOT'] . "/site/header.php.$lmajor")) :
		$lang = $lmajor;
		break;
	endif;
}

$ar = explode(".", $REQUEST_URI);
if ($ar[sizeof($ar) - 2] == "php") :
	$l = $ar[sizeof($ar) - 1];
	if(file_exists($_SERVER['DOCUMENT_ROOT'] . "/site/header.php.$l")) :
		$lang = $l;
	endif;
endif;

include($_SERVER['DOCUMENT_ROOT'] . "/site/header.php.$lang");

?>
