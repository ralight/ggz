<?php

$lang = "en";
$ar = split(", ", $_SERVER['HTTP_ACCEPT_LANGUAGE']);

foreach ($ar as $l)
{
	if(file_exists($_SERVER['DOCUMENT_ROOT'] . "/site/footer.php.$l")) :
		$lang = $l;
		break;
	endif;
}

$ar = explode(".", $REQUEST_URI);
if ($ar[sizeof($ar) - 2] == "php") :
	$l = $ar[sizeof($ar) - 1];
	if(file_exists($_SERVER['DOCUMENT_ROOT'] . "/site/footer.php.$l")) :
		$lang = $l;
	endif;
endif;

include($_SERVER['DOCUMENT_ROOT'] . "/site/footer.php.$lang");

?>
