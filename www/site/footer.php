<?php

$lang = "en";
$ar = split(", ", $HTTP_ACCEPT_LANGUAGE);

foreach ($ar as $l)
{
	if(file_exists("$DOCUMENT_ROOT/site/footer.php.$l")) :
		$lang = $l;
		break;
	endif;
}

include("footer.php.$lang");

?>
