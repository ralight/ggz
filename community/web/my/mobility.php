<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");
include_once("auth.php");
include_once("player.php");

if (!Auth::username()) :
	exit;
else:
	$ggzuser = Auth::username();
endif;

$p = new Player(Auth::username());

if ($_GET["export"] == "xml") :
	header("Content-type: text/xml");
	header("Content-disposition: attachment; filename=myggzcommunity.xml");
	$p->exportxml();
	exit;
endif;

if ($_GET["import"] == "xml") :
	if ($_FILES["xmlfile"]) :
		$p->importxml($_FILES["xmlfile"]["tmp_name"]);
	endif;
endif;

?>

<?php

$global_leftbar = "disabled";
include("top.inc");

?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Personal page (My Community): Personal data mobility
		<span class="itemleader"> :: </span>
		<a name="personal"></a>
	</h1>
	<div class="text">
	As an open gaming platform, <?php echo Config::getvalue("name"); ?> allows
	you to take your personal data with you and import it into other instances
	of the GGZ Community software. Configure your GGZ account once and take it
	whereever you want.
	</div>

	<div class="text">
	<a href="?export=xml">Export personal data as XML</a>
	</div>

	<div class="text">
	Of course data can also be imported again from other sites.
	Warning: This will overwrite your current settings, so export it first using
	the link above to have a backup ready.
	<br>
	GGZ personal data XML file:
	<br>
	<form action="?import=xml" method="POST" enctype="multipart/form-data">
	<input type="file" name="xmlfile">
	<br>
	<input type="submit" value="Import">
	</form>
	</div>
</div>

<?php include("bottom.inc"); ?>
