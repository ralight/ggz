<?php include("top.inc"); ?>

<?php
	$ret = Locale::includefile("articles/frontpage.inc");
	if (!$ret) :
		if (!$ret) :
			echo "Welcome! No frontpage has been created yet.";
		endif;
	endif;
?>

<?php include("bottom.inc"); ?>
