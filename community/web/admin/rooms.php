<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");
$global_leftbar = "disabled";
$global_rightbar = "disabled";
include("top.inc");

?>

<div id="main">
	<h1 style="color:#e05040">
		<span class="itemleader">:: </span>
		Administrative Commands - Room Configuration
		<span class="itemleader"> :: </span>
		<a name="personal"></a>
	</h1>
	<div class="text">
	Based on the room definition file in the configuration directory of the
	GGZ server, room variants can dynamically be created here.
	</div>
	<div class="text">

<?php
	$room_name = $_POST["room_name"];
	$clonedroom_name = $_POST["clonedroom_name"];
	$name = $_POST["name"];

	if ($clonedroom_name) :
		$res = $database->exec("UPDATE rooms SET action = 'delete' " .
			"WHERE filebased = 'f' AND name = '%^'",
			array($clonedroom_name));
	endif;
?>

<?php if (!$room_name) : ?>

	Select the room which you want to clone.
	<br/>
	<form action="rooms.php" method="POST">

<?php

$res = $database->exec("SELECT * FROM rooms WHERE filebased = 't'", array());
if (($res) && ($database->numrows($res) > 0)) :
	echo "<select name='room_name'>";
	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$name = $database->result($res, $i, "name");
		echo "<option>$name</option>";
	}
	echo "</select>";
?>

	<br/><br/>
	<input type="submit" value="Create cloned room..."/>
	</form>

<?php

else :
	echo "No rooms were found.";
endif;

?>

	</div>
	<div class="text">

	Select the cloned room which you want to delete.
	<br/>
	<form action="rooms.php" method="POST">

<?php

$res = $database->exec("SELECT * FROM rooms WHERE filebased = 'f' AND action IS NULL", array());
if (($res) && ($database->numrows($res) > 0)) :
	echo "<select name='clonedroom_name'>";
	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$name = $database->result($res, $i, "name");
		echo "<option>$name</option>";
	}
	echo "</select>";
?>

	<br/><br/>
	<input type="submit" value="Delete cloned room"/>
	</form>

<?php

else :
	echo "No cloned rooms were found.";
endif;

?>

<?php elseif (!$name) : ?>

	Configure the cloned room's parameters.
	<br/>
	<form action="rooms.php" method="POST">
	<input type="hidden" name="room_name" value="<?php echo $room_name; ?>"/>

<?php

$res = $database->exec("SELECT * FROM rooms WHERE filebased = 't' AND name='%^'",
	array($room_name));
if (($res) && ($database->numrows($res) == 1)) :
	$description = $database->result($res, 0, "description");
	$maxplayers = $database->result($res, 0, "maxplayers");
	$maxtables = $database->result($res, 0, "maxtables");
	$entryrestriction = $database->result($res, 0, "entryrestriction");

	echo "<table>";
	echo "<tr><th>Parameter</th><th>Original</th><th>New</th></tr>";
	echo "<tr><th>Name</td><td>$room_name</td><td><input name='name' value='$room_name'/></td></tr>";
	echo "<tr><th>Description</td><td>$description</td><td><input name='description' value='$description'/></td></tr>";
	echo "<tr><th>Max # players</td><td>$maxplayers</td><td><input name='maxplayers' value='$maxplayers'/></td></tr>";
	echo "<tr><th>Max # tables</td><td>$maxtables</td><td><input name='maxtables' value='$maxtables'/></td></tr>";
	echo "<tr><th>Entry restriction</td><td>$entryrestriction</td><td><select name='entryrestriction'>";
	echo "<option value='none'>None</option>";
	echo "<option value='registered'>Registered players only</option>";
	echo "<option value='admin'>Administrators only</option>";
	echo "</select></td></tr>";
	echo "</table>";
?>

	<br/><br/>
	<input type="submit" value="Clone room"/>
	</form>

<?php

else :
	echo "Room was not found.";
endif;

?>

<?php else : ?>

<?php
	$description = $_POST["description"];
	$maxplayers = $_POST["maxplayers"];
	$maxtables = $_POST["maxtables"];
	$entryrestriction = $_POST["entryrestriction"];

	$database->exec("INSERT INTO rooms " .
		"(filebased, name, description, gametype, maxplayers, maxtables, entryrestriction, action) VALUES " .
		"('f', '%^', '%^', '???', %^, %^, '%^', 'add')",
		array($name, $description, $maxplayers, $maxtables, $entryrestriction));
?>

	The room has been created.
	<a href="rooms.php">Back to room page</a>.

<?php endif; ?>

	</div>
</div>

<?php

include("bottom.inc")

?>
