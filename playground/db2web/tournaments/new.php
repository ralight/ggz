<?php include("head.inc"); ?>

<table border=0 cellspacing=0 cellpadding=1><tr><td bgcolor='#000000'>
<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>

<b>New tournament</b>
<br><br>

<form method="post" action="new2.php">

Name of the tournament:
<input type="text" name="form_name">

<br><br>

Game type:
<select name="form_game">
<option>TicTacToe</option>
</select>

<br><br>

Start date (DD/MM/YYYY):
<input type="text" name="form_date">

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

<input type="submit" value="Next page">

</form>

</td></tr></table>
</td></tr></table>

<?php include("foot.inc"); ?>
