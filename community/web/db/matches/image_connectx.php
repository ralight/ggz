<?php

include("savegame_connectx.php");
$savegame = new Savegame();
$savegame->load($savegamefile);

$im = imagecreatetruecolor($savegame->width * 50, $savegame->height * 50);

$background_color = imagecolorallocate($im, 183, 140, 131);
$red_color = imagecolorallocate($im, 255, 0, 0);
$yellow_color = imagecolorallocate($im, 255, 255, 0);

imagefill($im, 0, 0, $background_color);

for ($j = 0; $j < $savegame->height; $j++)
{
	for ($i = 0; $i < $savegame->width; $i++)
	{
		$cell = $savegame->game[$i][$j];
		if ($cell) :
			if ($cell == 1) :
				$color = $red_color;
			elseif ($cell == 2) :
				$color = $yellow_color;
			endif;
			imagefilledellipse($im, $i * 50 + 25, $j * 50 + 25, 30, 30, $color);
		endif;
	}
}

header("Content-type: image/png");
imagepng($im);
imagedestroy($im);

?>
