<form method="post" action="index.php?page=new2">

Name of the tournament:
<input type="text" name="form_name">

<br><br>

Tournament room:
<select name="form_room">
<?php
$res = $database->exec("SELECT DISTINCT name FROM rooms", NULL);
for ($i = 0; $i < $database->numrows($res); $i++)
{
	$roomname = $database->result($res, $i, "name");

	echo "<option>$roomname</option>\n";
}
?>
</select>

<br><br>

Start date (MM/DD/YYYY):
<?php
$startdate = date("m/d/Y", time());
echo "<input type='text' name='form_date' value='$startdate'>\n";
?>

<br><br>

Number of participants:
<select name="form_participants">
<?php
for($i = 2; $i <= 32; $i++)
{
	echo "<option>$i</option>\n";
}
?>
</select>

<br><br>

<i>
If the number of participants is not a power of 2 (i.e. 2, 4, 8, ...),
a certain number of AI players will join the games.
</i>

<br><br>

Participation mode:

<select name="form_mode">
<option value="fixed">Assignment by organizer</option>
<option value="signup">Signup by participants</option>
</select>

<br><br>

<input type="submit" value="Next page">

</form>

