<?php include_once("database.php"); ?>
<?php include_once("auth.php"); ?>
<?php

if(!Auth::username()) :
	header("Location: index.php");
	exit;
endif;

if ($task) :
	$ggzuser = Auth::username();
	$longitude = $long;
	$latitude = $lat;
	$res = $database->exec("UPDATE userinfo " .
		"SET longitude = $longitude, latitude = $latitude " .
		"WHERE handle = '$ggzuser'");
	header("Location: index.php");
	exit;
endif;

?>

<?php $global_leftbar = "disabled"; ?>
<?php $global_rightbar = "disabled"; ?>
<?php include("top.inc"); ?>

<script type="text/javascript">

function hover()
{
var map = document.getElementById("map");
//window.statusbar.??? = "quuuux";
mousex = window.event.x - map.x;
mousey = window.event.y - map.y;
}

function round(x)
{
return (x);
}

function click()
{
map_width = 596;
map_height = 331;
calibrate_x = -10;
calibrate_y = -20;
longpart = 360 * (mousex / map_width);
latpart = 180 * (mousey / map_height);
longitude = round((longpart - calibrate_x) - 180);
latitude = round(90 - (latpart + calibrate_y));
s = "Set coordinates to:\n";
s = s + "Longitude: " + longitude + "\n";
s = s + "Latitude: " + latitude;
alert(s);
window.location = "change.php?task=change&long=" + longitude + "&lat=" + latitude;
}

</script>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		World map - Modification
		<span class="itemleader"> :: </span>
		<a name="articles"></a>
	</h1>
	<div class="text">
	Click on your location on the map in order to update the database
	with the correct coordinates.
	</div>
	<h2>
		<span class="itemleader">:: </span>
		Selection map
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
	<img src="genmap.php?highlight=<?php echo Auth::username(); ?>" id="map" border="0" onmousemove="hover()" onclick="click()">
	</div>
</div>

<?php include("bottom.inc"); ?>
