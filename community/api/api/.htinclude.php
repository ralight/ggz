<?php

function parseconfigfile($configfile)
{
	$f = @fopen($configfile, "r");
	if(!$f) :
		return null;
	endif;

	$section = "";
	$content = array();
	while(!feof($f))
	{
		$a = fgets($f);
		if ($a == "\n") continue;
		$a = substr($a, 0, strlen($a) - 1);
		if ($a[0] == "#") continue;
		if (($a[0] == "[") and ($a[strlen($a) - 1] == "]")) :
			$section = substr($a, 1, strlen($a) - 2);
			continue;
		endif;
		#echo "//$section// $a<br>";
		$entry = explode("=", $a);
		$key = trim($entry[0]);
		$value = trim($entry[1]);
		#echo "::$key::$value::<br>";
		$content[$section][$key] = $value;
	}
	fclose($f);

	return $content;
}

?>
