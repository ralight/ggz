<?php include("top.inc"); ?>
<?php include("database.php"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Team management: New team
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
	<form action='teams.php?create=1' method='POST'>
	<table>
	<tr><td>Team name:</td><td><input type='input' name='team_name' value=''></td></tr>
	<tr><td>Full name:</td><td><input type='input' name='team_full' value=''></td></tr>
	<tr><td>Homepage:</td><td><input type='input' name='team_homepage' value=''></td></tr>
	<tr><td>Team logo:</td><td><input type='input' name='team_logo' value=''></td></tr>
	<tr><td></td><td><input type='submit' value='Create'></td></tr>
	</table>
	</form>
	</div>
</div>

<?php include("bottom.inc"); ?>
