<?php

$lang = "en";

$matches = array();
// Mozilla/5.0 (X11; U; Linux i686; pt-BR; rv:1.6) Gecko/20040413 Debian/1.6-5 
if(preg_match("/^Mozilla\/5.0 \(\S+; U; \S+ \S+; (\S+); \S+\) .*/", $_SERVER['HTTP_USER_AGENT'], $matches)) :
	$lang = $matches[1];
endif;

$langs = $_SERVER['HTTP_ACCEPT_LANGUAGE'];
$langs = str_replace(" ", "", $langs);
$langs = preg_replace("/;[^,]*/", "", $langs);

$langs = "$lang,$langs";
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
