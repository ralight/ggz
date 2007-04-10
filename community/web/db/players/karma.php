<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

include("top.inc");
include("stats.php");

?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Community Database: Players
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<?php
		include("karma.inc");
	?>
</div>

<?php include("bottom.inc"); ?>
