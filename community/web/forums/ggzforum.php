<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

include("top.inc");

?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Community Forums
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<?php
		if ($lookup) :
			if ($thread) :
				include("forum.inc");
			else :
				include("forumthreads.inc");
			endif;
		else :
			include("forumslist.inc");
		endif;
	?>
</div>

<?php include("bottom.inc"); ?>
