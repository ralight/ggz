<form method="post" action="index.php?page=new3">

<?php
for($i = 0; $i < $form_participants; $i++)
{
	echo "Name of player $i:";
	echo "<input type='text' name='form_participant_$i'>\n";
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
<input type="hidden" name="form_game" value="<?php echo $form_game; ?>">
<input type="hidden" name="form_date" value="<?php echo $form_date; ?>">
<input type="hidden" name="form_participants" value="<?php echo $form_participants; ?>">

</form>
