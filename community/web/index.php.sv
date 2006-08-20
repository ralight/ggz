<?php
	include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

	include("top.inc");

	$ret = Locale::includefile("articles/frontpage.inc");
	if (!$ret) :
		if (!$ret) :
			echo "Welcome! No frontpage has been created yet.";
		endif;
	endif;

	include("bottom.inc");
?>
