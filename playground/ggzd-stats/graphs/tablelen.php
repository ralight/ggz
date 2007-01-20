<?php

/*
 * File: control.c
 * Author: Roger Light
 * Project: ggzd stats
 * Date: 2007/01/20/
 * Desc: Plot something
 * $Id: tablelen.php 8958 2007-01-20 12:02:32Z oojah $
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

$datafile = "/var/www/localhost/include/ggzd/tablelen";
$data = file($datafile);

$timediv = 25;
$maxtime = 7200/$timediv;
$alltimes = array_fill(0, $maxtime, 0);

$file = "/var/www/localhost/include/ggzd/tablelen_processed";

if(filemtime($datafile) > filemtime($file)){
	foreach($data as $d){
		if(strpos($d, "TABLE_START") !== FALSE){
			$longtime = substr($d, 0, strpos($d, "TABLE_START")-1);
			$player = trim(substr($d, strpos($d, "TABLE_START")+strlen("TABLE_START")));
			$time = strtotime($longtime);
			$tables["$player"] = $time;
		}else if(strpos($d, "TABLE_END") !== FALSE){
			$longtime = substr($d, 0, strpos($d, "TABLE_END")-1);
			$player = trim(substr($d, strpos($d, "TABLE_END")+strlen("TABLE_END")));
			$time = strtotime($longtime);
			$timediff = ($time-$tables["$player"])/$timediv;
			if($timediff <= $maxtime){
				$alltimes[$timediff]++;
			}else{
				//print("$timediff<br/>");
			}
		}
	}

	$fptr = fopen($file, "w");
	if($fptr){
		for($i=0; $i<$maxtime; $i++){
			fputs($fptr, $alltimes[$i]."\n");
		}
	}
	fclose($fptr);
}

require("./graph_create.php");

$interval = 600/$timediv;
$title = "Table Duration Distribution (2 hours max, 10 minute intervals)";
$datalimit = $maxtime;
$roll = 0;

$tablelen = array(
	"name" => "tablelen",
	"label" => "",
	"file" => "$file",
	"scale" => 1,
	"index" => 0,
	"divby" => "",
	"plot" => true
);

$image = "images/tablelen.png";
$mtimediff = 3600*24;

graph_cached_create(array($tablelen), $interval, $title, $datalimit, $roll, $image, $mtimediff);

?>
