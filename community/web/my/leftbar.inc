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
		Visit <a href="/my/">My Community</a>.
	</form>

	<hr class="leather">

	<form class="leather" action="" method="get">
		<h1>My Community items</h1>
		<h2>Teams</h2>
		<a href="/db/teams/?lookup=myteam">My Team</a>
		<br>
		<h2>Game data</h2>
		<a href="/hotstuff/">Kamikaze level</a>
		<br>
	</form>
</div>

