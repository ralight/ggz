<?php include("top.inc"); ?>

<?php

include_once("database.php");
include_once("auth.php");

if (!Auth::username()) :
	exit;
else:
	$ggzuser = Auth::username();
endif;

?>

<?php
	if ($remove) :
		$database->exec("DELETE FROM karma WHERE fromhandle = '$ggzuser' AND tohandle = '$remove'");
	endif;
	if ($add) :
		$database->exec("INSERT INTO karma " .
			"(fromhandle, tohandle, karma) VALUES " .
			"('$ggzuser', '$form_player', $form_karma)");
	endif;
?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		New karma point
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
<form action='index.php?add=1' method='POST'>
Player name:<br>
<input type='text' name='form_player'><br>
Karma points:<br>
<select name='form_karma'>
<option value='1'>1 (good karma)</option>
<option value='0'>0 (bad karma)</option>
</select>
<br>
<input type='submit' value='Add'>
</form>
	</div>
	<h1>
		<span class="itemleader">:: </span>
		Spent karma points
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
<?php
	$res = $database->exec("SELECT * FROM karma WHERE fromhandle = '$ggzuser'");
	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$name = $database->result($res, $i, "tohandle");
		$karma = $database->result($res, $i, "karma");

		echo "$name: $karma points ";
		echo "(<a href='index.php?remove=$name'>delete</a>)";
		echo "<br>\n";
	}
?>
	</div>
</div>

<?php include("bottom.inc"); ?>
