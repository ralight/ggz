<?php include("top.inc"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Team management: Roles
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
<?php
$ggzuser = Auth::username();
$res = $database->exec("SELECT teamname FROM teammembers WHERE handle = '$ggzuser' AND role LIKE '%leader%'");
for ($i = 0; $i < $database->numrows($res); $i++)
{
$teamname = $database->result($res, $i, "teamname");

echo "<h2>$teamname</h2>";

	echo "<form action='teams.php?manage=1' method='POST'>\n";
	echo "<table>\n";
	echo "<tr><td>Player:</td><td>\n";
	echo "<input type='hidden' name='team_name' value='$teamname'>\n";
	echo "<select name='player_name'>\n";
	$res2 = $database->exec("SELECT * FROM teammembers WHERE teamname = '$teamname' AND role <> ''");
	for ($j = 0; $j < $database->numrows($res2); $j++)
	{
		$playername = $database->result($res2, $j, "handle");
		$role = $database->result($res2, $j, "role");

		$rolestr = "";
		if (strstr($role, "leader")) :
			$rolestr = "Leader";
		elseif (strstr($role, "vice")) :
			$rolestr = "Vice Leader";
		elseif (strstr($role, "member")) :
			$rolestr = "Member";
		endif;
		echo "<option value='$playername'>$playername ($rolestr)</option>\n";
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
}
?>
	</div>
</div>

<?php include("bottom.inc"); ?>
