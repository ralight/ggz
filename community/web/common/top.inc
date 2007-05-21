<?php
include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

include_once("accessadmin.php");
include_once("config.php");
include_once("locale.php");
include_once("negotiation.php");
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
	<title><?php Config::put("name"); ?></title>
	<link href="<?php Config::theme("full.css"); ?>" rel="stylesheet" type="text/css" title="All sidebars">
</head>

<body>
<?php include("navbar.php"); ?>

<div id="mainandacknowledgements">

<?php include("access.php"); ?>

