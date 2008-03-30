<?php

include("savegame_reversi.php");
$savegame = new Savegame();
$savegame->load($savegamefile);

$im = imagecreatetruecolor($savegame->width * 50, $savegame->height * 50);

$background_color = imagecolorallocate($im, 183, 180, 131);
$black_color = imagecolorallocate($im, 0, 0, 0);
$white_color = imagecolorallocate($im, 255, 255, 255);

imagefill($im, 0, 0, $background_color);

for ($j = 0; $j < $savegame->height; $j++)
{
	for ($i = 0; $i < $savegame->width; $i++)
	{
		$cell = $savegame->game[$i][$j];
		if ($cell) :
			if ($cell == 1) :
				$color = $black_color;
			elseif ($cell == 2) :
				$color = $white_color;
			endif;
			imagefilledellipse($im, $i * 50 + 25, $j * 50 + 25, 30, 30, $color);
		endif;
	}
}

header("Content-type: image/png");
imagepng($im);
imagedestroy($im);

?>
