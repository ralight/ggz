<?php $global_rightbar = "disabled"; ?>
<?php include("top.inc"); ?>
<?php include("database.php"); ?>
<?php include("live.php"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Community Database: Games
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
	<?php $live->output(); ?>
	</div>
</div>

<?php include("bottom.inc"); ?>