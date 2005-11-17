<?php include("top.inc"); ?>

<?php
	$ret = Locale::includefile("articles/frontpage.inc");
	if (!$ret) :
		$ret = Locale::includefile("articles/default.frontpage.inc");
		if (!$ret) :
			echo "¡Bienvenido! Todavía no existe una página initial.";
		endif;
	endif;
?>

<?php include("bottom.inc"); ?>
