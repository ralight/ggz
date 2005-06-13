<?php

include("savegame_freelords.php");
$savegame = new Savegame();
$savegame->load($savegamefile);

$im = imagecreatetruecolor($savegame->width * 4, $savegame->height * 4);

$map_colors = array();
$map_colors[0] = imagecolorallocate($im, 0, 202, 0);
$map_colors[1] = imagecolorallocate($im, 0, 0, 255);
$map_colors[2] = imagecolorallocate($im, 0, 161, 65);
$map_colors[3] = imagecolorallocate($im, 148, 101, 82);
$map_colors[4] = imagecolorallocate($im, 0, 0, 0);
$map_colors[5] = imagecolorallocate($im, 148, 80, 49);

$player_colors = array();
$player_colors[0] = imagecolorallocate($im, 255, 0, 0);
$player_colors[1] = imagecolorallocate($im, 255, 255, 0);
$player_colors[2] = imagecolorallocate($im, 0, 255, 0);
$player_colors[3] = imagecolorallocate($im, 0, 255, 255);
$player_colors[4] = imagecolorallocate($im, 255, 128, 0);
$player_colors[5] = imagecolorallocate($im, 255, 255, 128);
$player_colors[6] = imagecolorallocate($im, 128, 255, 0);
$player_colors[7] = imagecolorallocate($im, 128, 255, 255);

for ($j = 0; $j < $savegame->height; $j++)
{
	for ($i = 0; $i < $savegame->width; $i++)
	{
		$entry = $savegame->game[$i][$j];
		$colour = $map_colors[$entry];
		imagefilledrectangle($im, $i * 4, $j * 4, $i * 4 + 3, $j * 4 + 3, $colour);
	}
}

for ($j = 0; $j < $savegame->height; $j++)
{
	for ($i = 0; $i < $savegame->width; $i++)
	{
		$entry = $savegame->stacks[$i][$j];
		if ($entry) :
			$colour = $player_colors[$entry];
			imagefilledrectangle($im, $i * 4, $j * 4, $i * 4 + 3, $j * 4 + 3, $colour);
		endif;

		$entry = $savegame->cities[$i][$j];
		if ($entry) :
			$colour = $player_colors[$entry];
			imagerectangle($im, $i * 4 - 2, $j * 4 - 2, $i * 4 + 5, $j * 4 + 5, $colour);
			imagerectangle($im, $i * 4 - 3, $j * 4 - 3, $i * 4 + 6, $j * 4 + 6, $colour);
		endif;
	}
}

header("Content-type: image/png");
imagepng($im);
imagedestroy($im);

?>
