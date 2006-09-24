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

$datestamp = strtotime($form_date);
$datestring = date("d.m.Y", $datestamp);

echo "Tournament $form_name of $form_game will start on $datestring.\n";
echo "<br><br>\n";

$organizer = Auth::username();

$database->exec("INSERT INTO tournaments " .
	"(name, game, date, organizer) VALUES " .
	"('%^', '%^', '%^', '%^')", array($form_name, $gorm_game, $datestamp, $organizer));

$res = $database->exec("SELECT MAX(id) FROM tournaments", NULL);
$tid = $database->result($res, 0, "max");

$ar = random_array($form_participants);
for($i = 0; $i < $form_participants; $i++)
{
	$varname = "form_participant_$i";
	$name = $$varname;
	$number = $ar[$i];
	$database->exec("INSERT INTO tournamentplayers (id, number, name) VALUES ('%^', '%^', '%^')", array($tid, $number, $name));
	echo "Registered player $name as number $number.<br>\n";
}
?>

<br><br>

<input type="submit" value="Back to index">

</form>
