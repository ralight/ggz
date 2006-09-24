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

$nowtime = time();

for($day_index = 0; $day_index < 7; $day_index++){
	$thistime = $nowtime + $day_index * 86400;
	$datestring = date('y-m-d 00:00:00', $thistime);

	$day = date('d', $thistime);
	$month = date('m', $thistime);
	$year = date('Y', $thistime);

	$nowtimestring = date('d/m/y 00:00:00', $thistime);
	$nowdisplaystring = date('l jS F', $thistime);

	if ($thistime == strtotime(date('d/m/y 00:00:00'))){
		echo "<h3>Today ($nowdisplaystring)</h3>";
	}else{
		echo "<h3>$nowdisplaystring</h3>\n";
	}

	$tomorrowtime = $thistime + 86400;
	$yesterdaytime = $thistime - 86400;
	$week_day = date('l', $thistime);

	$res = $database->exec("SELECT 'onetime' AS type,id,name,room,date,description,organizer,style,rules FROM manual_tournaments ".
		"WHERE date >= '%^' ".
		"AND date < '%^' ".
		"UNION SELECT 'weekly' AS type,id,name,room,start,description,organizer,style,rules FROM weekly_manual_tournaments trny ".
		"WHERE (day = '%^') ".
		"AND (NOT EXISTS (SELECT id, date FROM canceled_manual_tournaments cncl ".
			"WHERE cncl.id = trny.id ".
			"AND date >= '%^' ".
			"AND date < '%^')) ".
		"AND ((start - time <= '%^' AND expire >= '%^') OR expire IS NULL) ORDER BY date",
		array($thistime, $tomorrowtime, $week_day, $thistime, $tomorrowtime, $thistime, $thistime));

	if($database->numrows($res) == 0){
		echo "No tournaments scheduled for this date.<br>";
	}else{
		echo "<ul>";
	
		for($i = 0; $i < $database->numrows($res); $i++){
			$id = $database->result($res, $i, "id");
			$type = $database->result($res, $i, "type");
			$name = $database->result($res, $i, "name");
			$room = $database->result($res, $i, "room");
			$datetime = $database->result($res, $i, "date");
			$display_time_string = date('g:i a e', $datetime);
			$desc = $database->result($res, $i, "description");
			$organizer = $database->result($res, $i, "organizer");
			$style = $database->result($res, $i, "style");
			$rules = $database->result($res, $i, "rules");
			//$format = $database->result($res, $i, "format");
			//$qualifiers = $database->result($res, $i, "qualifiers");
		
			print("<li");
			if(strlen($style)) print(" class=\"$style\"");
			print(">$display_time_string $name $desc in $room. Host: $organizer");
			if(strlen($rules)) print(" (<a href=\"$rules\">Rules</a>)");
			print("</li>\n");
		}
	}
	echo "</ul>\n";
}

?>

</body></html>
