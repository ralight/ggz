<?php include("top.inc"); ?>
<?php include("database.php"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Team management: Applications
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
<?php
$ggzuser = Auth::username();
$res = pg_exec($id, "SELECT teamname FROM teammembers WHERE handle = '$ggzuser' AND role LIKE '%leader%'");
for ($i = 0; $i < pg_numrows($res); $i++)
{
$teamname = pg_result($res, $i, "teamname");
echo "<h2>$teamname</h2>";

	$res2 = pg_exec($id, "SELECT * FROM teammembers WHERE teamname = '$teamname' AND role = ''");

	if (pg_numrows($res2) > 0) :
		//echo "<table><tr><td bgcolor='#00a0a0'>\n";

		echo "<form action='teams.php?approve=1' method='POST'>\n";
		echo "<table>\n";
		echo "<tr><td>Player:</td><td>\n";
		echo "<input type='hidden' name='team_name' value='$teamname'>\n";
		echo "<select name='player_name'>\n";
		for ($j = 0; $j < pg_numrows($res2); $j++)
		{
			$playername = pg_result($res2, $j, "handle");
			echo "<option>$playername</option>\n";
		}
		echo "</select>\n";
		echo "</td></tr>\n";
		echo "<tr><td>Approval:</td><td>\n";
		echo "<select name='player_approval'>\n";
		echo "<option>approved</option>\n";
		echo "<option>declined</option>\n";
		echo "</select>\n";
		echo "</td></tr>\n";
		echo "<tr><td></td><td><input type='submit' value='Approve'></td></tr>\n";
		echo "</table>\n";
		echo "</form>\n";

		//echo "</td></tr></table>\n";
	else :
		echo "No applications pending.\n";
	endif;

}
?>
	</div>
</div>

<?php include("bottom.inc"); ?>
