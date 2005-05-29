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

for ($j = 0; $j < $savegame->height; $j++)
{
	for ($i = 0; $i < $savegame->width; $i++)
	{
		$entry = $savegame->game[$i][$j];
		$colour = $map_colors[$entry];

		imagefilledrectangle($im, $i * 4, $j * 4, $i * 4 + 3, $j * 4 + 3, $colour);
	}
}

header("Content-type: image/png");
imagepng($im);
imagedestroy($im);

?>
