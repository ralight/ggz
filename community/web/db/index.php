<?php include("top.inc"); ?>
<?php include("database.php"); ?>
<?php include("stats.php"); ?>

<?php $stat->calculateGeneral(); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Community Database
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
	The database pages keep information about players, games,
	matches, rankings, and other statistics.
	</div>
	<h1>
		<span class="itemleader">:: </span>
		Database Overview
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">

	Database version <b><?php echo $stat->version() ?></b><br>
	<b><?php echo $stat->players_number ?></b> players have registered with this server.<br>
	<b><?php echo $stat->teams_number ?></b> teams have been founded.<br>
	<b><?php echo $stat->rankings_number ?></b> records are available in the ranking database.<br>
	<b><?php echo $stat->tournaments_number ?></b> tournaments have been organized.<br>
	<b><?php echo $stat->matches_number ?></b> individual games have been played.<br>
	<b><?php echo $stat->games_number ?></b> game types are available in the database.<br>
	</div>
</div>

<?php include("bottom.inc"); ?>
