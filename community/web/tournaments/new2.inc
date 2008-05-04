<form method="post" action="index.php?page=new3">

<?php

$freeform = false;

if (!$freeform) :
	$res = $database->exec("SELECT handle FROM users ORDER BY handle DESC", null);
endif;

for($i = 0; $i < $form_participants; $i++)
{
	echo "Name of player $i:";
	if ($freeform) :
		echo "<input type='text' name='form_participant_$i'>\n";
	else :
		echo "<select name='form_participant_$i'>\n";
		for ($j = 0; $j < $database->numrows($res); $j++)
		{
			$handle = $database->result($res, $j, "handle");
			echo "<option>$handle</option>\n";
		}
		echo "</select>\n";
	endif;
	echo "<br>\n";
}
?>

<br>

<i>
All players must be registered on the GGZ server.
</i>

<br><br>

<input type="submit" value="Next page">

<input type="hidden" name="form_name" value="<?php echo $form_name; ?>">
<input type="hidden" name="form_room" value="<?php echo $form_room; ?>">
<input type="hidden" name="form_date" value="<?php echo $form_date; ?>">
<input type="hidden" name="form_participants" value="<?php echo $form_participants; ?>">

</form>
