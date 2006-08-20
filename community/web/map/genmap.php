<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

include_once("database.php");

header("Content-type: image/png");
header("Cache-Control: no-cache, must-revalidate");
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");

$im = imagecreatefrompng("mundomap.large.png");

$yellow = imagecolorallocate($im, 255, 200, 0);
$red = imagecolorallocate($im, 255, 0, 0);
$orange = imagecolorallocate($im, 255, 150, 0);
$white = imagecolorallocate($im, 255, 255, 255);
$black = imagecolorallocate($im, 0, 0, 0);

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

	$color = $red;
	if ($handle == $highlight) :
		$color = $orange;
	endif;

	imagerectangle($im, $xpos - 5, $ypos - 5, $xpos + 6, $ypos + 6, $black);
	imagefilledrectangle($im, $xpos - 4, $ypos - 4, $xpos + 5, $ypos + 5, $white);
	imagefilledrectangle($im, $xpos - 3, $ypos - 3, $xpos + 4, $ypos + 4, $color);
}

imagepng($im);

?>
