<?php include("top.inc"); ?>
<?php include("database.php"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Team management: Join or leave a team
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
<form action='teams.php?join=1' method='POST'>
<table>
<tr><td>Team name:</td><td>
<select name='team_name'>
<?php
$res = pg_exec($id, "SELECT * FROM teams");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$teamname = pg_result($res, $i, "teamname");
	echo "<option>$teamname</option>\n";
}
?>
</select>
</td></tr>
<tr><td></td><td><input type='submit' value='Join'>
<input type='submit' value='Leave'></td></tr>
</table>
</form>
	</div>
</div>

<?php include("bottom.inc"); ?>
