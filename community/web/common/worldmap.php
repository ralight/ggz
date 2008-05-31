<?php if (Config::feature("worldmap")) : ?>
	<hr class="leather">

	<div class="menubar">
		<h1>_(Players worldwide)</h1>
<?php
if(@stat($_SERVER['DOCUMENT_ROOT'] . Config::gettheme("mundomap.png"))) :
include_once(Config::gettheme("mundomap.map"));
?>
		<a href="/map/"><img src="<?php Config::theme("mundomap.png"); ?>" usemap="#ggzmap" border="0" alt="world map"></a>
<?php
else :
?>
		_(World map not available.)
<?php
endif;
?>
	</div>
<?php endif; ?>
