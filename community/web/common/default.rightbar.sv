<div id="rightbar">
	<h1>Nyheter</h1>

	<hr class="leather">

	<div class="menubar">
		<h1>Matcher/Turneringar</h1>
<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");
include_once("latest.php");
latest_matches();
latest_tournaments();

?>
	</div>

	<hr class="leather">

	<div class="menubar">
		<h1>Individuella bloggar</h1>
<?php
latest_blogs();
?>
	</div>

	<hr class="leather">

	<div class="menubar">
		<h1>Inlägg i forum</h1>
<?php
latest_forumposts();
?>
	</div>

<?php
include_once("worldmap.php");
?>

</div>
