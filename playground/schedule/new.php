<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head><title>GGZ Schedule</title>
<link rel="stylesheet" href="style.css" type="text/css" />
<link rel="shortcut icon" href="/ggz_fav.png" />
</head>
<body>
<h1>GGZ Tournament Creation</h1>

<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");
date_default_timezone_set('US/Pacific');
include_once("auth.php");

$organizer = Auth::username();
$invalid_field = "";

// Check if this is a post attempt
if(isset($_POST['type'])){
	$is_valid = true;
	
	// Validate input fields
	
	if (($_POST['type'] != 'weekly') && ($_POST['type'] != 'onetime')){
		$is_valid = false;
		$message = "Invalid tournament type!";
		$invalid_field = "type";
	}
	
	if (($_POST["expires"] != 'unlimited') && ($_POST["expires"] != 'limited')){
		$is_valid = false;
		$message = "Expires field invalid!";
		$invalid_field = "expires";
	}
	
	if (!preg_match("/^[a-zA-Z0-9-_: ]+$/", $_POST["name"])){
		$is_valid = false;
		$message = "Name is invalid!";
		$invalid_field = "name";
	}
	
	if (!preg_match("/^[a-zA-Z0-9-_:,*.\" \/]+$/", $_POST["desc"])){
		$is_valid = false;
		$message = "Description is invalid!";
		$invalid_field = "description";
	}
	
	if (!preg_match("/^[a-zA-Z0-9-_: ]+$/", $_POST["game"])){
		$is_valid = false;
		$message = "Game is invalid!";
		$invalid_field = "game";
	}
	
	if (!preg_match("/^[a-zA-Z0-9-_: \/]+$/", $_POST["room"])){
		$is_valid = false;
		$message = "Room is invalid!";
		$invalid_field = "room";
	}
	
	if (!preg_match("/^[0-9]{4}$/", $_POST["start_year"])){
		$is_valid = false;
		$message = "Year is invalid!";
		$invalid_field = "start_date";
	}
	
	if (!preg_match("/^[0-9]{2}$/", $_POST["start_month"])){
		$is_valid = false;
		$message = "Month is invalid!";
		$invalid_field = "start_date";
	}
	
	if (!preg_match("/^[0-9]{2}$/", $_POST["start_day"])){
		$is_valid = false;
		$message = "Day is invalid!";
		$invalid_field = "start_date";
	}
	
	if (!preg_match("/^[0-9]{2}$/", $_POST["hour"])){
		$is_valid = false;
		$message = "Hour is invalid!";
		$invalid_field = "time";
	}
	
	if (!preg_match("/^[0-9]{2}$/", $_POST["minutes"])){
		$is_valid = false;
		$message = "Minutes is invalid!";
		$invalid_field = "time";
	}
	
	if (!preg_match("/^[0-9]([0-9])?$/", $_POST["weeks"])){
		$is_valid = false;
		$message = "Weeks is invalid!";
		$invalid_field = "weeks";
	}
	
	if ($is_valid == true){
		$start_datestamp = strtotime($_POST["start_year"]."-".$_POST["start_month"]."-".$_POST["start_day"]." ".$_POST["hour"].":".$_POST["minutes"].":00");
	
		$now = strtotime("now");
	
		if ($start_datestamp < $now)
		{
			$message = "Can't create tourneys in past time!<br>";
			$invalid_field = "start_date";
		}else{
			include_once('game-specific-rules/'.$_POST['game'].'.php');
	
			$week_day = date('l', $start_datestamp);
			$startday_datestamp = strtotime($_POST["start_year"]."-".$_POST["start_month"]."-".$_POST["start_day"]." 00:00:00");
			$time = $start_datestamp - $startday_datestamp;
			$nextday_datestamp = strtotime("tomorrow", $startday_datestamp);
			
			if ($_POST["type"] == 'weekly'){
				print_r($_POST);
				if ($_POST["expires"] == "unlimited"){
					$expires_datestamp = "NULL";
				}else{
					$expires_datestamp = strtotime("+".$weeks." weeks", $start_datestamp);
				}
	
					print("expires: $expires_datestamp<br/>");
				// Check if tourney exceeds maximum number per slot
				$res = $database->exec("SELECT id FROM manual_tournaments ".
						"WHERE day = '%^' ".
						"AND time = '%^' ".
						"AND date >= '%^' ".
						"AND date <= '%^' ".
						"UNION SELECT id FROM weekly_manual_tournaments ".
						"WHERE day = '%^' ".
						"AND time = '%^' ".
						"AND start >= '%^' ".
						"AND (expire <= '%^' OR expire IS NULL)", 
						array($week_day, $time, $start_datastamp, $expires_datestamp, $week_day, $time, $start_datestamp, $expires_datestamp));
	
				if ($database->numrows($res) >= $max_tournaments_per_slot){
					$message = "Can't create more than $max_tournaments_per_slot tourneys per time slot!<br>";
					$invalid_field = "start_date";
				}else{
					// Check if tourney exceeds maximum number per day per host

					// If the tourney is weekly limited, just check each day of tourney from start 
					// end, just like checking a one time tourney, if it's unlimited, check until the
					// latest tourney time in system, and then check "up to inifinty" time seperately
					$curr_check_datestamp = $start_datestamp;

					if ($_POST["expires"] == "unlimited"){
						$res = $database->exec("SELECT MAX(date) FROM manual_tournaments", NULL);

						if ($database->numrows($res) == 1){
							$max_manual_datestamp = $database->result($res, 1, "date");
							$expires_datestamp = $max_manual_datestamp;
						}

						$res = $database->exec("SELECT MAX(expire) FROM weekly_manual_tournaments WHERE expire IS NOT NULL", NULL);

						if ($database->numrows($res) == 1){
							$max_weekly_datestamp = $database->result($res, 1, "expire");

							if ($max_weekly_datestamp > $max_manual_datestamp){
								$expires_datestamp = $max_weekly_datestamp;
							}
						}
					}

					$is_valid = true;

					print("expires: $expires_datestamp<br/>");
					if ($expires_datestamp != "NULL"){
						while ($curr_check_datestamp <= $expires_datetsamp && $is_valid == true){
							$curr_nextday_datestamp = strtotime("tomorrow", $curr_check_datestamp);

							// Check if tourney exceeds maximum number per day per host
							$res = $database->exec("SELECT id FROM manual_tournaments ".
								"WHERE day = '%^' ".
								"AND date >= '%^' ".
								"AND date <= '%^' ".
								"UNION SELECT id FROM weekly_manual_tournaments ".
								"WHERE day = '%^' ".
								"AND start >= '%^' ".
								"AND (expire >= '%^' OR expire IS NULL)",
								array($week_day, $curr_check_datestamp, $curr_nextday_datestamp, $week_day, $curr_check_datestamp, $curr_nextday_datestamp));
	
							if (($max_tournaments_per_day != 'unlimited') && ($database->numrows($res) >= $max_tournaments_per_day)){
								$message = "Can't create more than $max_tournaments_per_day tourneys per host per day!";
								$invalid_field = "start_date";
								$is_valid = false;
							}else{
								$curr_check_datestamp = strtotime("+1 week", $curr_check_datestamp);
							}
						}
					}

					// Add tourney
					if ($is_valid == true){
						$database->exec("INSERT INTO weekly_manual_tournaments " .
							"(name, description, game, room, start, expire, day, time, organizer) VALUES " .
							"('%^', '%^', '%^', '%^', '%^', '%^', '%^', '%^', '%^')",
							array($_POST['name'], $_POST['desc'], $_POST['game'], $_POST['room'], $start_datestamp, $expires_datestamp, $week_day, $time, $organizer));

						echo "<p class='highlight'>Tournament was added successfully!</p>";
					}
				}
			}else if ($_POST["type"] == 'onetime'){
				// Check if tourney exceeds maximum number per slot
				$res = $database->exec("SELECT id FROM manual_tournaments ".
					"WHERE date = '%^' ".
					"UNION SELECT id FROM weekly_manual_tournaments WHERE ".
					"day = '%^' ".
					"AND time = '%^' ".
					"AND start <= '%^' ".
					"AND (expire >= '%^' OR expire IS NULL)",
					array($start_datestamp, $week_day, $time, $start_datestamp, $start_datestamp));
	
				if ($database->numrows($res) >= $max_tournaments_per_slot)
				{
					$message = "Can't create more than $max_tournaments_per_slot tourneys per time slot!<br>";
					$invalid_field = "start_date";
				}else{
					// Check if tourney exceeds maximum number per day per host
					$res = $database->exec("SELECT id FROM manual_tournaments ".
						"WHERE day = '%^' ".
						"AND date >= '%^' ".
						"AND date <= '%^' ".
						"UNION SELECT id FROM weekly_manual_tournaments ".
						"WHERE day = '%^' ".
						"AND start >= '%^' ".
						"AND (expire >= '%^' OR expire IS NULL)",
						array($week_day, $startday_datestamp, $nextday_datestamp, $week_day, $start_datestamp, $start_datestamp));
	
					if (($max_tournaments_per_day != 'unlimited') && ($database->numrows($res) >= $max_tournaments_per_day))
					{
						$message = "Can't create more than $max_tournaments_per_day tourneys per host per day!";
						$invalid_field = "start_date";
					}
					else
					{
						$database->exec("INSERT INTO manual_tournaments ".
							"(name, description, game, room, date, day, time, organizer) VALUES ".
							"('%^', '%^', '%^', '%^', '%^', '%^', '%^', '%^')",
							array($_POST['name'], $_POST['desc'], $_POST['game'], $_POST['room'], $start_datestamp, $week_day, $time, $organizer));
	
						echo "<p class='highlight'>Tournament was added successfully!</p>";
					}
				}
			}
		}
	}
}

?>

<script TYPE="text/javascript"> 
	function onChangeTourneyType(value)
	{
		if (value == "weekly")
		{
			document.getElementById("weekly_limited").style.display = 'block';
			document.getElementById("weekly_unlimited").style.display = 'block';
			document.getElementById("date_header").innerHTML = 'Start Date:';
		}
		else if (value == "onetime")
		{
			document.getElementById("weekly_limited").style.display = 'none';
			document.getElementById("weekly_unlimited").style.display = 'none';
			document.getElementById("date_header").innerHTML = 'Date:';
		}
	}
</script>

<form method="post" action="new.php" id="form_game">

<table class="hidden">

<tr><td class="hidden">
Game type: <select name="game"><option value="spades">Spades</Option>

<?php
$res = $database->exec("SELECT DISTINCT game FROM stats");
for ($i = 0; $i < $database->numrows($res); $i++)
{
	$game = $database->result($res, $i, "game");

	echo "<option>$game</option>\n";
}
?>

</select>
</tr></td>

<?php

if ($invalid_field == "name")
{
	echo "<tr><td class='input-error'>$message";
	$invalid_field = "";
}
else
{
	echo "<tr><td class='hidden'>";
}
?>

Tournament Name: <input type="text" cols="60" name="name">
</td></tr>

<?php

if ($invalid_field == "description")
{
	echo "<tr><td class='input-error'>$message";
	$invalid_field = "";
}
else
{
	echo "<tr><td class='hidden'>";
}
?>

Tournament Description:
</td></tr>

<tr><td class="hidden">
<textarea rows="4" cols="40" name="desc"></textarea>
</td></tr>

Room: <select name="room">

<?php

	$res = $database->exec("SELECT name FROM manual_tournaments_rooms", NULL);
	for ($i = 0; $i < $database->numrows($res); $i++){
		$name = $database->result($res, $i, "name");
		echo "<option value='$name'>$name</option>\n";
	}

?>

</select>
</td></tr>

<tr><td class="hidden">

Type:
<select name="type" onChange="onChangeTourneyType(this.value);">
<option value="weekly" SELECTED>Weekly</option>
<option value="onetime">One Time</option>
</select>
</td></tr>

<?php

if ($invalid_field == "start_date")
{
	echo "<tr><td class='input-error'>$message";
	$invalid_field = "";
}
else
{
	echo "<tr><td class='hidden'>";
}
?>

<span id="date_header">Start Date:</span>
<?php

if ($year)
{
	$startdatestamp = strtotime("$year-$month-$day $hour:$minutes:00");
	$startdate = getdate($startdatestamp);
}
else
{
	$startdate = getdate();

	if ($startdate['minutes'] >= 30){
		$date = strtotime("+30 minutes");
		$startdate = getdate($date);
		$startdate['minutes'] = 0;
	}else{
		$startdate['minutes'] = 30;
	}
}

echo '<select name="start_day">';
for ($day = 1; $day <= 31; $day++){
	if ($day == $startdate['mday']){
		printf ("<option value='%02d' SELECTED>%02d</option>\n", $day, $day);
	}else{
		printf ("<option value='%02d' >%02d</option>\n", $day, $day);
	}
}
echo '</select>';
?>

<?php
echo '<select name="start_month">';
for ($month = 1; $month <= 12; $month++){
	if ($month == $startdate['mon']){
		printf ("<option value='%02d' SELECTED>%02d</option>\n", $month, $month);
	}else{
		printf ("<option value='%02d' >%02d</option>\n", $month, $month);
	}
}
echo '</select>';
?>

<select name="start_year">
<?php
for ($year = $startdate['year']; $year <= $startdate['year'] + 5; $year++){
	echo "<option value='$year' >$year</option>\n";
}
?>
</select>
</td></tr>

<tr id="weekly_limited"><td class="hidden">
<input type="radio" name="expires" value="limited" checked="checked">Limited to: </input>
<select name="weeks">

<?php
for ($week = 1; $week <= 28; $week++)
{
	printf ("<option value='%02d' SELECTED>%02d</option>\n", $week, $week);
}
?>

</select> Weeks
</td></tr>

<tr id="weekly_unlimited"><td class="hidden">
<input type="radio" name="expires" value="unlimited">Unlimited </input>
</td></tr>

<tr><td class="hidden">

Time:

<?php
echo '<select name="hour">';
for ($hour = 0; $hour <= 23; $hour++)
{
	if ($hour == $startdate['hours'])
	{
		printf ("<option value='%02d' SELECTED>%02d</option>\n", $hour, $hour);
	}else{
		printf ("<option value='%02d'>%02d</option>\n", $hour, $hour);
	}
}
echo '</select>';

echo '<select name="minutes">';

for ($minutes = 0; $minutes <= 30; $minutes += 30){
	if ($startdate['minutes'] == $minutes){
		printf ("<option value='%02d' SELECTED>%02d</option>\n", $minutes, $minutes);
	}else{
		printf ("<option value='%02d'>%02d</option>\n", $minutes, $minutes);
	}
}

echo '</select> US/Pacific';
?>
</td></tr>
</table>

<br><br>

<input type="submit" value="Submit">

</form>

<?php
	include("links.php");
?>
</body></html>
