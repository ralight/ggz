<?php include("top.inc"); ?>
<?php include("database.php"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Team management: Join or leave a team
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<h2>
		<span class="itemleader">:: </span>
		Join a team
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<form action='teams.php?join=1' method='POST'>
<table>
<tr><td>Team name:</td><td>
<select name='team_name'>
<?php
$ggzuser = Auth::username();
$res = $database->exec("SELECT DISTINCT teamname FROM teammembers WHERE teamname NOT IN " .
	"(SELECT teamname FROM teammembers WHERE handle = '$ggzuser')");
for ($i = 0; $i < $database->numrows($res); $i++)
{
	$teamname = $database->result($res, $i, "teamname");

	echo "<option>$teamname</option>\n";
}
?>
</select>
</td></tr>
<tr><td></td><td><input type='submit' value='Join'></td></tr>
</table>
</form>
	</div>
	<h2>
		<span class="itemleader">:: </span>
		Leave a team
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<form action='teams.php?join=1' method='POST'>
<table>
<tr><td>Team name:</td><td>
<select name='team_name'>
<?php
$ggzuser = Auth::username();
$res = $database->exec("SELECT DISTINCT teamname FROM teammembers WHERE handle = '$ggzuser'");
for ($i = 0; $i < $database->numrows($res); $i++)
{
	$teamname = $database->result($res, $i, "teamname");

	echo "<option>$teamname</option>\n";
}
?>
</select>
</td></tr>
<tr><td></td><td><input type='submit' value='Leave'></td></tr>
</table>
</form>
	</div>
</div>

<?php include("bottom.inc"); ?>
