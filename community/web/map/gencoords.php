<?php include_once("database.php"); ?>
<?php

echo "<map name='ggzmap'>";

$res = $database->exec("SELECT handle, longitude, latitude FROM userinfo " .
	"WHERE latitude IS NOT NULL");
for ($i = 0; $i < $database->numrows($res); $i++)
{
	$handle = $database->result($res, $i, "handle");
	$longitude = $database->result($res, $i, "longitude");
	$latitude = $database->result($res, $i, "latitude");

	list($mapwidth, $mapheight, $type, $attr) = getimagesize("mundomap.large.png");
	$longnorm = $longitude + 180;
	$latnorm = $latitude + 90;
	$calibrate_x = -10;
	$calibrate_y = -20;
	$xpos = (int)(($longnorm + $calibrate_x) * $mapwidth / 360);
	$ypos = $mapheight - (int)(($latnorm + $calibrate_y) * $mapheight / 180);

	$x = $xpos - 4;
	$x2 = $xpos + 4;
	$y = $ypos - 4;
	$y2 = $ypos + 4;

	echo "<area coords='$x,$y,$x2,$y2'";
	echo "alt='$handle' title='$handle'";
	echo "href='/db/players/?lookup=$handle'>";

}

echo "</map>";

?>
