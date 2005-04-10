<?php include("top.inc"); ?>
<?php include("database.php"); ?>
<?php include("stats.php"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Community Database: Tournaments
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<?php
		if ($lookup) :
			include("information.inc");
		else :
			include("intro.inc");
		endif;
	?>
</div>

<?php include("bottom.inc"); ?>
