<?php

//echo $REQUEST_URI;
//phpinfo();

//if ($HTTP_HOST == "rankings.ggzgamingzone.org") :
//	header("Location: http://mindx.dyndns.org/ggz/db2web$REQUEST_URI");
//elseif ($HTTP_HOST == "dev.ggzgamingzone.org") :
//	header("Location: http://sourceforge.net/projects/ggz$REQUEST_URI");
//else :
//	header("Location: http://ggz.sourceforge.net$REQUEST_URI");
//	#header("Location: http://beta.ggzgamingzone.org/demo.php");
//endif;

if ($HTTP_HOST == "live.ggzgamingzone.org") :
	header("Location: http://www.ggzgamingzone.org/live.php");
else :
	header("Location: http://www.ggzgamingzone.org");
endif;

exit;

?>
