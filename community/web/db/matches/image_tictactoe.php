<?php

$cross = array(
	3, 3,
	13, 3,
	25, 15,
	35, 3,
	45, 3,
	29, 25,
	45, 45,
	35, 45,
	25, 35,
	13, 45,
	3, 45,
	21, 25
);
# 50x50
$circle = array(
	3, 25,
	25, 3,
	47, 25,
	25, 47,
	3, 25,
	13, 25,
	25, 37,
	37, 25,
	25, 13,
	13, 25
);

include("savegame_tictactoe.php");
$savegame = new Savegame();
$savegame->load($savegamefile);

$im = imagecreatetruecolor($savegame->width * 50, $savegame->height * 50);

$background_color = imagecolorallocate($im, 233, 180, 91);
$circle_color = imagecolorallocate($im, 200, 0, 0);
$cross_color = imagecolorallocate($im, 0, 0, 200);

imagefill($im, 0, 0, $background_color);

for ($j = 0; $j < $savegame->height; $j++)
{
	for ($i = 0; $i < $savegame->width; $i++)
	{
		$cell = $savegame->game[$i][$j];
		if ($cell) :
			if ($cell == 1) :
				$polygon = $cross;
				$color = $cross_color;
			elseif ($cell == 2) :
				$polygon = $circle;
				$color = $circle_color;
			endif;
			for ($k = 0; $k < sizeof($polygon); $k += 2)
			{
				$polygon[$k] += $i * 50;
				$polygon[$k + 1] += $j * 50;
			}
			imagefilledpolygon($im, $polygon, sizeof($polygon) / 2, $color);
		endif;
	}
}

header("Content-type: image/png");
imagepng($im);
imagedestroy($im);

?>
