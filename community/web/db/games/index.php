<?php include("top.inc"); ?>
<?php include("database.php"); ?>
<?php include("stats.php"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Community Database: Games
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<h2>
		<span class="itemleader">:: </span>
		<?php stats_gamesheader($id, $lookup); ?>
		<span class="itemleader"> :: </span>
	</h2>
	<div class="text">
	<?php stats_games($id, $lookup); ?>
	</div>
</div>

<?php include("bottom.inc"); ?>
