<?php include("top.inc"); ?>
<?php include("database.php"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Team management: Roles
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
<?php
	echo "<form action='teams.php?manage=1' method='POST'>\n";
	echo "<table>\n";
	echo "<tr><td>Player:</td><td>\n";
	echo "<input type='hidden' name='team_name' value='$teamname'>\n";
	echo "<select name='player_name'>\n";
	$res = pg_exec($id, "SELECT * FROM teammembers WHERE teamname = '$teamname' AND role <> ''");
	for ($i = 0; $i < pg_numrows($res); $i++)
	{
		$playername = pg_result($res, $i, "handle");
		$role = pg_result($res, $i, "role");
		echo "<option value='$playername'>$playername ($role)</option>\n";
	}
	echo "</select>\n";
	echo "</td></tr>\n";
	echo "<tr><td>Assign role:</td><td>\n";
	echo "<select name='player_role'>\n";
	echo "<option value='member'>Member</option>\n";
	echo "<option value='vice'>Vice Leader</option>\n";
	echo "<option value='leader'>Leader</option>\n";
	echo "</select>\n";
	echo "</td></tr>\n";
	echo "<tr><td></td><td><input type='submit' value='Change'></td></tr>\n";
	echo "</table>\n";
	echo "</form>\n";
?>
	</div>
</div>

<?php include("bottom.inc"); ?>
