<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head><title>GGZ Schedule</title>
<link rel="stylesheet" href="style.css" type="text/css" />
<link rel="shortcut icon" href="/ggz_fav.png" />
</head>
<body>

<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");
include_once("auth.php");
date_default_timezone_set('US/Pacific');

print("<h1>Weekly Tournaments</h1>");

$days = array('Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday');

foreach ($days as $curr_day)
{
	echo "<h2>$curr_day</h2>";

	$res = $database->exec("SELECT * FROM weekly_manual_tournaments WHERE day = '%^' ORDER BY time", array($curr_day));

	if($database->numrows($res) == 0){
		echo "No tournaments scheduled for this day.<br>";
	}else{
		echo "<table border='thin'><tr><th>Time</th><th>Name</th><th>Room</th><th>Description</th></tr>";
	}	

	for($i = 0; $i < $database->numrows($res); $i++)
	{
		echo "<tr>\n";
		$name = $database->result($res, $i, "name");
		$room = $database->result($res, $i, "room");
		$start_date = $database->result($res, $i, "start");
		$time = date('H:i e', $start_date);
		$desc = $database->result($res, $i, "description");

		echo "<td width=10%>$time</td><td width=20%>$name</td><td width=20%>$room</td><td width=50%>$desc</td>\n";
		echo "</tr>\n";
	}

	echo "</table>";
}

	include("links.php");
?>

</body></html>
