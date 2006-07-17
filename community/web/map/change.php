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

function hover(event)
{
var map = document.getElementById("map");
var pointer = document.getElementById("pointer");

s = "pagex " + event.pageX + " clientx " + event.clientX + " mapx " + map.x;
s += "; "
s += "pagey " + event.pageY + " clienty " + event.clientY + " mapy " + map.y;
//alert(s);

if(event.x)
{
	// Konqueror
	// =^ clientX/clientY in firefox - wrong!
	s = "eventx " + event.x + " eventy " + event.y;
	//alert(s);
	//mousex = window.event.x - map.x;
	//mousey = window.event.y - map.y;
	mousex = event.pageX - map.x;
	mousey = event.pageY - map.y;
	pointerx = mousex + map.x / 2 + 3; // FIXME: hack?
	pointery = mousey + map.y / 2 + 3;
}
else
{
	// Firefox
	mousex = event.pageX - map.x - map.x;
	mousey = event.pageY - map.y - map.y;
	pointerx = mousex + map.x;
	pointery = mousey + map.y;
}

if(true)
{
	pointer.style.left = pointerx + "px";
	pointer.style.top = pointery + "px";
}

if(false)
{
	alert("mousex " + mousex + " mousey " + mousey);
}

//window.defaultStatus = "quux-bar";
//window.status = "quuux";
}

function round(x)
{
return (x);
}

function click(event)
{
var map = document.getElementById("map");
var pointer = document.getElementById("pointer");

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
window.location.href = "change.php?task=change&long=" + longitude + "&lat=" + latitude;
}

function init()
{
	document.images["map"].onmousemove = hover;
	document.images["map"].onmousedown = click;

	if(true)
	{
		var map = document.getElementById("map");
		var pointer = document.getElementById("pointer");

		pointer.style.width = 10;
		pointer.style.height = 10;
		pointer.style.backgroundColor = "#ffb0b0";
		pointer.style.position = "absolute";
		pointer.style.left = map.x + "px";
		pointer.style.top = map.y + "px";
	}
}

window.onload = init;

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
	This functionality requires JavaScript to be enabled!
	</div>
	<h2>
		<span class="itemleader">:: </span>
		Selection map
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
	<img src="genmap.php?highlight=<?php echo Auth::username(); ?>" id="map" border="0">
	<div id="pointer"></div>
	</div>
</div>

<?php include("bottom.inc"); ?>
