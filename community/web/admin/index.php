<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");
$global_leftbar = "disabled";
$global_rightbar = "disabled";
include("top.inc");

?>

<div id="main">
	<h1 style="color:#e05040">
		<span class="itemleader">:: </span>
		Administrative Commands
		<span class="itemleader"> :: </span>
		<a name="personal"></a>
	</h1>
	<div class="text">
	As an administrator, you might want to work with the following tools
	to ensure a smooth operation of <?php Config::put("name") ?>.
	</div>
	<div class="text">

<?php

include_once("database.php");

if ($_GET["check"] == "mail") :
	$email = Config::getvalue("mail");
	mail($email, "GGZ Community Admin: test mail", "");
	echo "Sent mail to $email<br>\n";
elseif ($_GET["check"] == "phpinfo") :
	phpinfo();
else:
	echo "<a href='index.php?check=mail'>test admin mail</a><br>";
	echo "<a href='index.php?check=phpinfo'>phpinfo output</a><br>";
	echo "<a href='rooms.php'>room configuration</a><br>";
endif;

?>

	</div>
</div>

<?php

include("bottom.inc")

?>
