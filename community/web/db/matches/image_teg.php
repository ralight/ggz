<?php

include("savegame_teg.php");
$savegame = new Savegame();
$savegame->load($savegamefile);

$im = imagecreatetruecolor(1000, 450);

$background_color = imagecolorallocate($im, 133, 80, 90);

$player_colors = array();
$player_colors[0] = imagecolorallocate($im, 200, 80, 90);
$player_colors[1] = imagecolorallocate($im, 90, 200, 80);
$player_colors[2] = imagecolorallocate($im, 80, 90, 200);

imagefill($im, 0, 0, $background_color);

$filename = "IRL_warboard.png";
$piece = imagecreatefrompng("teg/$filename");
$sizes = getimagesize("teg/$filename");
imagecopy($im, $piece, 0, 0, 0, 0, $sizes[0], $sizes[0]);

include("helper_teg.php");
$countries = teg_loadmap();

for ($i = 0; $i < sizeof($savegame->game); $i++)
{
	$entry = $savegame->game[$i];

	$country = $entry['country'];
	$player = $entry['player'];
	$armies = $entry['armies'];

	$data = $countries[$country];

	$filename = $data['file'];
	$piece = imagecreatefrompng("teg/$filename");
	$sizes = getimagesize("teg/$filename");
	$x = $data['posx'];
	$y = $data['posy'];
	if ($filename) :
		imagecopy($im, $piece, $x, $y, 0, 0, $sizes[0], $sizes[1]);

		$ax = $x + ($sizes[0] + $data['armyx']) / 2;
		$ay = $y + ($sizes[1] + $data['armyy']) / 2;
		$size = $armies * 6 + 10;
		imagefilledellipse($im, $ax, $ay, $size, $size, $player_colors[$player]);
	endif;
}

$ims = imagecreatetruecolor(500, 225);
imagecopyresized($ims, $im, 0, 0, 0, 0, 500, 225, 1000, 450);

header("Content-type: image/png");
imagepng($ims);
imagedestroy($ims);

?>
