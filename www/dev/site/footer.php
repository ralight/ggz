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

include($_SERVER['DOCUMENT_ROOT'] . "/site/footer.php.$lang");

?>
