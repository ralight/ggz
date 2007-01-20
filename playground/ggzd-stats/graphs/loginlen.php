<?php

/*
 * File: control.c
 * Author: Roger Light
 * Project: ggzd stats
 * Date: 2007/01/20/
 * Desc: Plot something
 * $Id: loginlen.php 8958 2007-01-20 12:02:32Z oojah $
 *
 * Copyright (C) 1999 Roger Light.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

function timetosecs($time)
{
	/* Convert HH:MM:SS to S */

	$chunks = explode(":", $time);

	switch(count($chunks)){
		case 1:
			return $time;
		case 2:
			return 60*$chunks[0] + $chunks[1];
		case 3:
			return 3600*$chunks[0] + 60*$chunks[1] + $chunks[2];
		default:
			return 0;
	}
}

$datafile = "/var/www/localhost/include/ggzd/loginlen";
$data = file($datafile);

$file = "/var/www/localhost/include/ggzd/loginlen_processed";
$image = "images/loginlen.png";


$split = 60*2;
$currentsplit = $split;
$currentindex = 0;
$alltimes[0] = 0;

if(filemtime($datafile) > filemtime($file)){
	foreach($data as $d){
		$secs = timetosecs($d);
		if($secs > $currentsplit){
			$currentsplit += $split;
			$currentindex++;
			$alltimes[$currentindex] = 0;
			if($secs > 3600*12) break;
		}
		$alltimes[$currentindex]++;
	}

	$fptr = fopen("/var/www/localhost/include/ggzd/loginlen_processed", "w");
	if($fptr){
		for($i=0; $i<$currentindex; $i++){
			fputs($fptr, $alltimes[$i]."\n");
		}
	}
	fclose($fptr);
}

require("./graph_create.php");

$interval = 60/2;
$title = "Login Duration Distribution (12 hour max, 1 hour intervals)";
$datalimit = 800;
$roll = 0;

$loginlen = array(
	"name" => "loginlen",
	"label" => "",
	"file" => "/var/www/localhost/include/ggzd/loginlen_processed",
	"scale" => 1,
	"index" => 0,
	"divby" => "",
	"plot" => true
);

$image = "images/loginlen.png";
$mtimediff = 3600*24;

graph_cached_create(array($loginlen), $interval, $title, $datalimit, $roll, $image, $mtimediff);

?>
