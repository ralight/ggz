<?php $global_leftbar = "disabled"; ?>
<?php $global_rightbar = "disabled"; ?>
<?php include("top.inc"); ?>

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
	if you are logged in.
	</div>
	<h2>
		<span class="itemleader">:: </span>
		The current map
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php
include_once("map/mundomap.map");
?>
	<img src="mundomap.large.png" usemap="#ggzmap" border="0">
	</div>
</div>

<?php include("bottom.inc"); ?>
