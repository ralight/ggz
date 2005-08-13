<?php

include("savegame_dots.php");
$savegame = new Savegame();
$savegame->load($savegamefile);

$factor = 40;

$im = imagecreatetruecolor(($savegame->width - 1) * $factor + 1, ($savegame->height - 1) * $factor + 1);

$background_color = imagecolorallocate($im, 33, 180, 91);
$area_color_1 = imagecolorallocate($im, 200, 0, 0);
$area_color_2 = imagecolorallocate($im, 200, 100, 0);
$line_color = imagecolorallocate($im, 0, 0, 200);

imagefill($im, 0, 0, $background_color);

for ($j = 0; $j < $savegame->height; $j++)
{
	for ($i = 0; $i < $savegame->width; $i++)
	{
		$cell = $savegame->game[$i][$j];
		$owner = $savegame->owners[$i][$j];
		if ($owner) :
			if ($owner == "player1") :
				$area_color = $area_color_1;
			else:
				$area_color = $area_color_2;
			endif;
			imagefilledrectangle($im, $i * $factor, $j * $factor,
				$i * $factor + $factor - 1, $j * $factor + $factor - 1, $area_color);
		endif;
		if (($cell == "horizontal") || ($cell == "both")) :
			imageline($im, $i * $factor, $j * $factor,
				$i * $factor + $factor - 1, $j * $factor, $line_color);
		endif;
		if (($cell == "vertical") || ($cell == "both")) :
			imageline($im, $i * $factor, $j * $factor,
				$i * $factor, $j * $factor + $factor - 1, $line_color);
		endif;
	}
}

header("Content-type: image/png");
imagepng($im);
imagedestroy($im);

?>
