<?php

$latest = "0.0.14";

$uri = $_SERVER['REQUEST_URI'];

$uri = preg_replace("/\/releases\/latest\//", "", $uri);
$uri = str_replace("/", "", $uri);

header("Location: /releases/$latest/$uri");

?>
