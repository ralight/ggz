<?php

$defaultlang = "en-us";
// from header.php/footer.php: $page="header"/"footer"
// from i18n.php: $page=""

$matches = array();
// Mozilla/5.0 (X11; U; Linux i686; pt-BR; rv:1.6) Gecko/20040413 Debian/1.6-5 
if(preg_match("/^Mozilla\/5.0 \(\S+; U; \S+ \S+; (\S+); \S+\) .*/", $_SERVER['HTTP_USER_AGENT'], $matches)) :
	$browserlang = $matches[1];
endif;

$acceptlangs = $_SERVER['HTTP_ACCEPT_LANGUAGE'];
$acceptlangs = str_replace(" ", "", $acceptlangs);
$acceptlangs = preg_replace("/;[^,]*/", "", $acceptlangs);

$langs = "$browserlang,$acceptlangs,$defaultlang";
$ar = split(",", $langs);

foreach ($ar as $l)
{
	if(file_exists($_SERVER['DOCUMENT_ROOT'] . "/site/$page.php.$l")) :
		$lang = $l;
		break;
	endif;

	$lmajor = preg_replace("/-.*/", "", $l);
	if(file_exists($_SERVER['DOCUMENT_ROOT'] . "/site/$page.php.$lmajor")) :
		$lang = $lmajor;
		break;
	endif;
}

$ar = explode(".", $REQUEST_URI);
if ($ar[sizeof($ar) - 2] == "php") :
	$l = $ar[sizeof($ar) - 1];
	if(file_exists($_SERVER['DOCUMENT_ROOT'] . "/site/$page.php.$l")) :
		$lang = $l;
	endif;
endif;

if ($page) :
	include($_SERVER['DOCUMENT_ROOT'] . "/site/$page.php.$lang");
endif;

?>
