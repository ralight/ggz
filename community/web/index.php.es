<?php include("top.inc"); ?>

<?php
	$ret = Locale::includefile("articles/frontpage.inc");
	if (!$ret) :
		echo "¡Bienvenido! Todavía no existe una página initial.";
	endif;
?>

<?php include("bottom.inc"); ?>
