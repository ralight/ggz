<br>

<?php
	$filepath = $_SERVER['PHP_SELF'];
	$filear = explode("/", $filepath);
	$file = $filear[sizeof($filear) - 1];

	$rightbar = str_replace("php", "rightbar", $file);
	$leftbar = str_replace("php", "leftbar", $file);

?>

<br>

<?php Locale::includefile("acknowledgements.inc"); ?>

</div>

<?php
	$mainwidth = 60;
	$mainleft = 20;

	if((!$global_leftbar) && (@include($leftbar))) :
	else :
		if ($global_leftbar != "disabled") :
			if (!$global_leftbar) :
				include_once("auth.php");
				if(Auth::username()) :
					Locale::includefile("my/leftbar.inc");
				else :
					Locale::includefile("default.leftbar");
				endif;
			else:
				Locale::includefile($global_leftbar);
			endif;
		else:
			$mainwidth += 19;
			$mainleft -= 19;
		endif;
	endif;

	if((!$global_rightbar) && (@include($rightbar))) :
	else :
		if ($global_rightbar != "disabled") :
			if (!$global_rightbar) :
				Locale::includefile("default.rightbar");
			else:
				Locale::includefile($global_rightbar);
			endif;
		else:
			$mainwidth += 19;
		endif;
	endif;

if ($mainwidth != 60) :
	echo "<style type=\"text/css\">\n";
	echo "#mainandacknowledgements\n";
	echo "{\n";
	echo "width: $mainwidth%;\n";
	echo "}\n";
	echo "body #mainandacknowledgements\n";
	echo "{\n";
	echo "left: $mainleft%;\n";
	echo "}\n";
	echo "</style>\n";
endif;

?>

</body>
</html>
