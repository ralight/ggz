<?php include("head.inc"); ?>

<table border=0 cellspacing=0 cellpadding=1><tr><td bgcolor='#000000'>
<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>

<b>Ready to start</b>
<br><br>

<form method="post" action="index.php">

<?php

function random_array($size)
{
	$x = array();
	$y = array();
	for($i = 0; $i < $size; $i++)
	{
		$x[$i] = $i;
	}

	$num = $size;
	for($i = 0; $i < $size; $i++)
	{
		$v = rand() % $num;
		$y[$i] = $x[$v];
		$x[$v] = $x[$num - 1];
		$num--;
	}
	return $y;
}

$conn = pg_connect("host=$dbhost dbname=$dbname user=$dbuser password=$dbpass");

$datestamp = strtotime($form_date);
$datestring = date("d.m.Y", $datestamp);

echo "Tournament $form_name of $form_game will start on $datestring.\n";
echo "<br><br>\n";

pg_exec($conn, "INSERT INTO tournaments (name, game, date) VALUES ('$form_name', '$form_game', $datestamp)");

$res = pg_exec($conn, "SELECT MAX(id) FROM tournaments");
$id = pg_result($res, 0, "max");

$ar = random_array($form_participants);
for($i = 0; $i < $form_participants; $i++)
{
	$varname = "form_participant_$i";
	$name = $$varname;
	$number = $ar[$i];
	pg_exec($conn, "INSERT INTO tournamentplayers (id, number, name) VALUES ($id, $number, '$name')");
	echo "Registered player $name as number $number.<br>\n";
}
?>

<br><br>

<input type="submit" value="Back to index">

</form>

</td></tr></table>
</td></tr></table>

<?php include("foot.inc"); ?>
