<?php

$records = 20;
$uptimes_file = "uptimes";
$cache_file = "uptimes.cache";
$daemon = "ggzd";
$hostname = "live.ggzgamingzone.org";

function format_time($seconds)
{
	$days = 0;
	$hours = 0;
	$minutes = 0;

	if($seconds > 86400){
		$days = floor($seconds / 86400);
		$seconds -= $days * 86400;
	}
	if($seconds > 3600){
		$hours = floor($seconds / 3600);
		$seconds -= $hours * 3600;
	}
	if($seconds > 60){
		$minutes = floor($seconds / 60);
		$seconds -= $minutes * 60;
	}
	if($hours < 10){
		$hours = "0$hours";
	}
	if($minutes < 10){
		$minutes = "0$minutes";
		}
	if($seconds < 10){
		$seconds = "0$seconds";
	}

	return "$days days, $hours:$minutes:$seconds";
}

if(file_exists($cache_file) && filemtime($cache_file) > filemtime($uptimes_file))
{
	print(file_get_contents($cache_file));
}else{
	$utimes = file($uptimes_file);

	$times = $utimes;

	$current = $utimes[count($utimes)-1];
	rsort($utimes, SORT_NUMERIC);
	arsort($times, SORT_NUMERIC);
	//print_r($times);

	$page = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	$page .= "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"DTD/xhtml1-strict.dtd\">\n";
	$page .= "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n";
	$page .= "<head><title>Uptime records for $daemon on $hostname</title>\n";
	$page .= "<link rel=\"stylesheet\" href=\"style.css\" type=\"text/css\"/></head><body>\n";
	$page .= "<h1>Uptime records for $daemon on $hostname</h1><table>\n";
	$page .= "<tr><th></th><th>#</th><th>Uptime</th></tr>\n";

	for($i = 0; $i < count($utimes); $i++){
		if($utimes[$i] == $current){
			$current_pos = $i;
			break;
		}
	}

	for($i = 0; $i < $records; $i++){
		if($i+1 == $current_pos){
			$page .= ("<tr class=\"current\"><td>-&gt;</td><td>".($i+1)."</td><td>".format_time($utimes[$i])."</td></tr>\n");
		}else{
			$page .= ("<tr><td></td><td>".($i+1)."</td><td>".format_time($utimes[$i])."</td></tr>\n");
		}
	}

	if($current_pos > $records){
		$page .= "<tr class=\"current border\"><td>-&gt;</td><td>$current_pos</td><td>".format_time($current)."</td></tr>\n";
	}

	if($current_pos > 0){
		$oneup = $utimes[$current_pos - 1] - $utimes[$current_pos];
		$top = $utimes[0] - $utimes[$current_pos];
	}else{
		$oneup = 0;
		$top = 0;
	}

	if($current_pos > 9){
		$topten = $utimes[9] - $utimes[$current_pos];
	}else{
		$topten = 0;
	}

	if($oneup > 0){
		$page .= "<tr class=\"border\"><td>1up</td><td>in</td><td>".format_time($oneup)."</td></tr>\n";
	}
	if($topten > 0){
		$page .= "<tr><td>t10</td><td>in</td><td>".format_time($topten)."</td></tr>\n";
	}
	if($top > 0){
		$page .= "<tr><td>no1</td><td>in</td><td>".format_time($top)."</td></tr>\n";
	}
	$page .= "</table></body></html>";

	file_put_contents($cache_file, $page);
	print($page);
}

?>
