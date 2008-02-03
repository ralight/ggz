<?php

include("savegame_go.php");
$savegame = new Savegame();
$savegame->load($savegamefile);

$im = imagecreatefrompng("go/cgobanboard.png");
$im_white = imagecreatefrompng("go/white.png");
$im_black = imagecreatefrompng("go/black.png");

$commentcolor = imagecolorallocate($im, 0, 0, 200);

for ($j = 0; $j < $savegame->height; $j++)
{
	for ($i = 0; $i < $savegame->width; $i++)
	{
		$cell = $savegame->game[$i][$j];

		if ($cell == "W") :
			imagecopy($im, $im_white, $i * 18 + 25 - 9, $j * 18 + 25 - 9, 0, 0, 18, 18);
		elseif ($cell == "B") :
			imagecopy($im, $im_black, $i * 18 + 25 - 9, $j * 18 + 25 - 9, 0, 0, 18, 18);
		endif;
	}
}

header("Content-type: image/png");
imagepng($im);
imagedestroy($im);

?>
