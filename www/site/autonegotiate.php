<?php

$x = $REQUEST_URI;
if($x[strlen($x) - 1] == "/"):
	$x .= "index.php";
endif;
$x .= ".en";

header("Location: $x");

?>
