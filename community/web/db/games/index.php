<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

include("top.inc"); 
include("stats.php");

?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Community Database: Games
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<?php
		if ($lookup) :
			include("information.inc");
		else :
			include("intro.inc");
		endif;
	?>
</div>

<?php include("bottom.inc"); ?>
