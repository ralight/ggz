<div id="pagehead">
<?php

Locale::includefile("pagehead.inc");
if(!$_SERVER["HTTPS"]) :
	Locale::includefile("tls.inc");
endif;
Locale::includefile("navbar.inc");

?>
	<div class="adblock">
		#pagehead .adblock
		<img src="" alt="#pagehead .adblock img">
	</div>
</div>
