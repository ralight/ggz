<?php include_once("header.inc"); ?>

<?php

$q = $_SERVER["REDIRECT_QUERY_STRING"];
$q = str_replace("q=", "", $q);
$q = strtolower($q);

?>

<?php include_once("searchbox.inc"); ?>

<?php include_once("navigation.inc"); ?>

<?php include_once("footer.inc"); ?>
