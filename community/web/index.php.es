<?php
	include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

	include("top.inc");

	$ret = Locale::includefile("articles/frontpage.inc");
	if (!$ret) :
		$ret = Locale::includefile("articles/default.frontpage.inc");
		if (!$ret) :
			echo "¡Bienvenido! Todavía no existe una página initial.";
		endif;
	endif;

	include("bottom.inc");
?>
