<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

include_once("database.php");
include_once("auth.php");

$database->exec("DELETE FROM tournamentplayers WHERE id = '%^' AND number = '%^'",
	array($tid, $number));
$database->exec("INSERT INTO tournamentplayers (id, number, name) " .
	"VALUES ('%^', '%^', '%^')",
	array($tid, $number, Auth::username()));

header("Location: index.php?page=show2&tid=$tid");

?>
