<?php

$lang = "en";
$ar = split(", ", $HTTP_ACCEPT_LANGUAGE);

foreach ($ar as $l)
{
	if(file_exists("$DOCUMENT_ROOT/site/header.php.$l")) :
		$lang = $l;
		break;
	endif;
}

include("header.php.$lang");

?>
