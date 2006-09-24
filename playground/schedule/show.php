<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head><title>GGZ Schedule</title>
<link rel="stylesheet" href="style.css" type="text/css" />
<link rel="shortcut icon" href="/ggz_fav.png" />
</head>
<body><h1>GGZ Tournament Schedule</h1>
<p>Hosts should email <a href="mailto:hosting@ggzcommunity.org">hosting@ggzcommunity.org</a> to register a timeslot for hosting. We are working on an automated scheduling system, stay tuned to see something new in a few weeks.</p>

<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");
include_once("database.php");

date_default_timezone_set('US/Pacific');

if (($_GET["day"] != null) && ($_GET["month"] != null) && ($_GET["year"] != null)){
	$nowtime = strtotime($_GET["year"]."-".$_GET["month"]."-".$_GET["day"]);

	$day = $_GET["day"];
	$month = $_GET["month"];
	$year = $_GET["year"];
}else{
	$datestring = date('y-m-d 00:00:00');
	$nowtime = strtotime($datestring);

	$day = date('d', $nowtime);
	$month = date('m', $nowtime);
	$year = date('Y', $nowtime);
}

if(isset($_GET["delete"])){
	if (is_numeric($_GET["delete"])){
		if ($_GET["type"] == 'weekly'){
			if ($_GET["method"] == 'cancel'){
				$database->exec("INSERT INTO canceled_manual_tournaments ".
					"(id, date) VALUES ".
					"('%^', '%^')",
					array($_GET['delete'], $_GET['tourney_timestamp']));
				echo "Weekly tournament canceled just for this date!";
			}else if ($_GET["method"] == 'delete'){
				$database->exec("DELETE FROM weekly_manual_tournaments WHERE id = '%^'",array($_GET['delete']));
				echo "Weekly tournament deleted!";
			}
		}else if ($_GET["type"] == 'onetime'){
			$database->exec("DELETE FROM manual_tournaments WHERE id = '%^'",array($_GET['delete']));
			echo "One-time tournament deleted!";
		}
	}
}

$nowtimestring = date('d/m/y 00:00:00', $nowtime);
$nowdisplaystring = date('d/m/y', $nowtime);

if ($nowtime == strtotime(date('d/m/y 00:00:00')))
{
	echo "<h3>Today's tournaments ($nowdisplaystring)</h3>";
}else{
	echo "<h3>Tournaments for $nowdisplaystring</h3>";
}

$tomorrowtime = strtotime('tomorrow', $nowtime);
$yesterdaytime = strtotime('yesterday', $nowtime);
$week_day = date('l', $nowtime);

$res = $database->exec("SELECT 'onetime' AS type, id, name, room, date, description, organizer FROM manual_tournaments ".
	"WHERE date >= '%^' ".
	"AND date < '%^' ".
	"UNION SELECT 'weekly' AS type, id, name, room, start, description, organizer FROM weekly_manual_tournaments trny ".
	"WHERE (day = '%^') ".
	"AND (NOT EXISTS (SELECT id, date FROM canceled_manual_tournaments cncl ".
		"WHERE cncl.id = trny.id ".
		"AND date >= '%^' ".
		"AND date < '%^')) ".
	"AND ((start - time <= '%^' AND expire >= '%^') OR expire IS NULL) ORDER BY date",
	array($nowtime, $tomorrowtime, $week_day, $nowtime, $tomorrowtime, $nowtime, $nowtime));

if($database->numrows($res) == 0)
{
	echo "No tournaments schedule for this date.<br>";
}else{
	echo "<table class='normal'><tr><th>Time</th><th>Name</th><th>Room</th><th>Description</th></tr>";

	for($i = 0; $i < $database->numrows($res); $i++){
		echo "<tr>\n";
		$id = $database->result($res, $i, "id");
		$type = $database->result($res, $i, "type");
		$name = $database->result($res, $i, "name");
		$room = $database->result($res, $i, "room");
		$datetime = $database->result($res, $i, "date");
		$display_time_string = date('H:i e', $datetime);
		$desc = $database->result($res, $i, "description");
		$organizer = $database->result($res, $i, "organizer");
	
		echo "<td width=10%>$display_time_string</td><td width=20%>$name</td><td width=20%>$room</td><td width=50%>$desc</td>\n";

		if (Auth::username() == $organizer){
			$timestring = date('H:i:00', $datetime);
			$datestring = date('Y-m-d', $nowtime);
			$curr_tourney_timestamp = strtotime($datestring." ".$timestring);

			if ($type == 'weekly'){
				echo "<td><a href='index.php?page=show&delete=$id&type=$type&method=delete&day=$day&month=$month&year=$year]'>Delete Weekly</a></td><td><a href='index.php?page=show&delete=$id&type=$type&method=cancel&tourney_timestamp=$curr_tourney_timestamp&day=$day&month=$month&year=$year'>One-time Cancelation</a></td>\n";
			}else if ($type == 'onetime'){
				echo "<td><a href='index.php?page=show&delete=$id&type=$type&method=delete&day=$day&month=$month&year=$year'>Delete</a></td>\n";
			}
		}

		echo "</tr>\n";
	}

echo "</table>";
}

include("date-jump.inc");
include("links.php");

?>

</body></html>
