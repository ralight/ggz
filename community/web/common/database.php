<?php

require_once(".htconf");

$id = @pg_connect("host=$dbhost dbname=$dbname user=$dbuser password=$dbpass");

if (($id) && ($ggzuser) && ($ggzpass)) :
	$res = pg_exec($id, "SELECT * FROM users WHERE password = '$ggzpass' AND handle = '$ggzuser'");
	if (($res) && (pg_numrows($res) == 1)) :
		$ggzuser = pg_result($res, 0, "handle");
	else :
		unset($ggzuser);
	endif;
endif;

?>
