<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

$global_leftbar = "disabled";
$global_rightbar = "disabled";
include("top.inc");

?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		World map
		<span class="itemleader"> :: </span>
		<a name="articles"></a>
	</h1>
	<div class="text">
	This is a map showing all players who registered their coordinates.
	You can do so on your <a href="/my/change.php">personal page</a>
	if you are logged in. Altering the location without knowing the
	coordinates is now possible by clicking <a href="change.php">here</a>.
	</div>
	<h2>
		<span class="itemleader">:: </span>
		The current map
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php
include_once("map/gencoords.php");
?>
	<img src="genmap.php" usemap="#ggzmap" border="0" alt="world map">
	</div>
</div>

<?php include("bottom.inc"); ?>
