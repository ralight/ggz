<?php

include("savegame_chess.php");
$savegame = new Savegame();
$savegame->load($savegamefile);

$im = imagecreatetruecolor($savegame->width * 50, $savegame->height * 50);

$background_color = imagecolorallocate($im, 133, 80, 90);
$alternate_color = imagecolorallocate($im, 200, 80, 90);

imagefill($im, 0, 0, $background_color);

for ($j = 0; $j < $savegame->height; $j++)
{
	for ($i = 0; $i < $savegame->width; $i++)
	{
		if (!(($i + $j) % 2)) :
			imagefilledrectangle($im,
				$i * 50, $j * 50, $i * 50 + 49, $j * 50 + 49, $alternate_color);
		endif;

		$cell = $savegame->game[$i][$j];

		if ($cell) :
			$ar = explode(":", $cell);
			$colour = $ar[0];
			$figure = $ar[1];
			if ($colour == "black") :
				$colour = "b";
			else :
				$colour = "w";
			endif;
			if ($figure == "R") :
				$figure = "rook";
			elseif ($figure == "Q") :
				$figure = "queen";
			elseif ($figure == "K") :
				$figure = "king";
			elseif ($figure == "B") :
				$figure = "bishop";
			elseif ($figure == "N") :
				$figure = "knight";
			elseif ($figure == "P") :
				$figure = "pawn";
			endif;
			$filename = "50." . $figure . "_" . $colour . ".svg.png";
			$piece = imagecreatefrompng("chess/$filename");
			imagecopy($im, $piece, $i * 50, $j * 50, 0, 0, 50, 50);
		endif;
	}
}

header("Content-type: image/png");
imagepng($im);
imagedestroy($im);

?>
