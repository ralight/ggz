<?php 

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");
$global_leftbar = "disabled";
$global_rightbar = "disabled";
include("top.inc");

?>
<?php
if(@stat("planet.inc")) :
?>
<div id="main">
<?php
	include("planet.inc");
?>
</div>
<?php
else :
	echo "Blog aggregation not configured.";
endif;
?>
<?php include("bottom.inc"); ?>
