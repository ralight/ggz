<?php include("head.inc"); ?>

<table border=0 cellspacing=0 cellpadding=1><tr><td bgcolor='#000000'>
<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>

<b>New players</b>
<br><br>

<form method="post" action="new3.php">

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

</td></tr></table>
</td></tr></table>

<?php include("foot.inc"); ?>
