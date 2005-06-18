<div id="pagehead">
<?php

Locale::includefile("pagehead.inc");
if (Config::getvalue("tls")) :
	if (!$_SERVER["HTTPS"]) :
		Locale::includefile("tls.inc");
	endif;
endif;
Locale::includefile("navbar.inc");

?>
</div>
