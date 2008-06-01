<div id="leftbar">
	<form class="leather" action="/login/" method="get">
		<h1>_(Personal sidebar)</h1>
		_(You are logged in as) <?php echo html(Auth::username()); ?>.
		<br>
		<input type="submit" size="10" maxlength="15" value="Logout" class="button">
	</form>
	
	<hr class="leather">

	<form class="leather" action="/login/" method="get">
		<h1>_(Personal page)</h1>
		_(Visit) <a href="/my/">_(My Community)</a>.<br>
		_(Change my) <a href="/my/change.php">_(settings)</a>.<br>
		_(Visit my) <a href="/db/players/?lookup=<?php echo htmlurl(Auth::username()); ?>">_(public page)</a>.
	</form>

<?php if (Config::feature("datarepo")) : ?>
	<hr class="leather">

	<form class="leather" action="" method="get">
		<h1>_(My Community items)</h1>
<?php
include_once("player.php");
$p = new Player(Auth::username());
$p->items();
?>
	</form>
<?php endif; ?>
</div>

