<?php
if(!$_POST['page']) $page = "menu";

if ($_POST['page'] == "menu") :
	$caption = "Tournament planning";
elseif ($_POST['page'] == "new") :
	$caption = "New tournament";
elseif ($_POST['page'] == "show") :
	$caption = "Existing tournaments";
elseif ($_POST['page'] == "show-weekly") :
	$caption = "Weekly tournaments";
elseif ($_POST['page'] == "show-empty") :
	$caption = "Empty tournaments slots";
else :
	$_POST['page'] = "menu";
endif;
?>
	<h1>
		<span class="itemleader">:: </span>
<?php echo $caption; ?>
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
<?php include($_POST['page'].".inc"); ?>
	</div>
