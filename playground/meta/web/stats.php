<?php include_once("header.inc"); ?>

<?php

$res = pg_exec($conn, "SELECT count(*) FROM games");
$num_games = pg_result($res, 0, "count");

$res = pg_exec($conn, "SELECT count(*) FROM zones");
$num_zonegames = pg_result($res, 0, "count");

$res = pg_exec($conn, "SELECT count(*) FROM metaservers");
$num_metaservers = pg_result($res, 0, "count");

$res = pg_exec($conn, "SELECT count(*) FROM gameservers");
$num_gameservers = pg_result($res, 0, "count");

?>

<div id="content">
	<h4>Statistics</h4>
	<p>
		You can find here some statistics about past and present game entries,
		most active meta servers and number of queries performed at Metacle.
		<br/><br/>
		<span class="contentBox">
			<form>
				Supported game types:
				<?php echo $num_games; ?> (of which
				<?php echo $num_zonegames; ?>
				can be played in a gaming centre)<br/>
				Aggregated meta servers:
				<?php echo $num_metaservers; ?><br/>
				Currently running game servers:
				<?php echo $num_gameservers; ?><br/>
			</form>
		</span>
	</p>

</div>

<?php include_once("footer.inc"); ?>
