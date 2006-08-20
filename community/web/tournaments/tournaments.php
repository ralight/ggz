<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

if(!$page) $page = "menu";

if ($page == "menu") :
	$caption = "Tournament planning";
elseif ($page == "new") :
	$caption = "New tournament";
elseif ($page == "new2") :
	$caption = "New player";
elseif ($page == "new3") :
	$caption = "Ready to start";
elseif ($page == "show") :
	$caption = "Existing tournaments";
elseif ($page == "show2") :
	$caption = "Existing tournaments";
else :
	$page = "";
endif;
?>
	<h1>
		<span class="itemleader">:: </span>
<?php echo $caption; ?>
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
<?php include("$page.inc"); ?>
	</div>
