<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

$active = 0;
if (preg_match("/^\/admin\//", $_SERVER["REQUEST_URI"])) :
	if (!preg_match("/^\/admin\/login.php/", $_SERVER["REQUEST_URI"])) :
		include_once("database.php");
		include_once("../admin/loginfunc.inc");
		$checked = checkauthentication(default_realm());
		if($checked['errorstr']) :
			header("Location: /admin/login.php");
			exit;
		endif;
	endif;
endif;

?>
