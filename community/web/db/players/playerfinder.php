<?php include("top.inc"); ?>
<?php include("database.php"); ?>
<?php include("stats.php"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Community Database: Player finder
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<?php
		if (!$find) :
			include("finderform.inc");
		else :
			include("finderresults.inc");
		endif;
	?>
</div>

<?php include("bottom.inc"); ?>
