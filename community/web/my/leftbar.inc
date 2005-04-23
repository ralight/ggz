<div id="leftbar">
	<form class="leather" action="/login/" method="get">
		<h1>Personal sidebar</h1>
		You are logged in as <?php echo Auth::username(); ?>.
		<br>
		<input type="submit" size="10" maxlength="15" value="Logout" class="button">
	</form>
	
	<hr class="leather">

	<form class="leather" action="/login/" method="get">
		<h1>Personal page</h1>
		Visit <a href="/my/">My Community</a>.<br>
		Change my <a href="/my/change.php">settings</a>.<br>
		Visit my <a href="/db/players/?lookup=<?php echo Auth::username(); ?>">public page</a>.
	</form>

	<hr class="leather">

	<form class="leather" action="" method="get">
		<h1>My Community items</h1>
<?php
include_once("player.php");
$p = new Player(Auth::username());
$p->items();
?>
	</form>
</div>

