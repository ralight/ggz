<?php

/*
 * File: control.c
 * Author: Roger Light
 * Project: ggzd stats
 * Date: 2007/01/20/
 * Desc: Plot something
 * $Id: load.php 8958 2007-01-20 12:02:32Z oojah $
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

require("./graph_create.php");

$interval = 288;
$title = "Load average";
$datalimit = 4036;
$roll = 0;

$load1min = array(
	"name" => "load1min",
	"label" => "1 min",
	"file" => "/var/log/ggzd/load.log",
	"scale" => 0.01,
	"index" => 1,
	"divby" => "",
	"plot" => true
);

$load5min = array(
	"name" => "load5min",
	"label" => "5 min",
	"file" => "/var/log/ggzd/load.log",
	"scale" => 0.01,
	"index" => 2,
	"divby" => "",
	"plot" => true
);

$load15min = array(
	"name" => "load15min",
	"label" => "15 min",
	"file" => "/var/log/ggzd/load.log",
	"scale" => 0.01,
	"index" => 3,
	"divby" => "",
	"plot" => true
);

$image = "images/load.png";
$mtimediff = 3600;

graph_cached_create(array($load1min, $load5min, $load15min), $interval, $title, $datalimit, $roll, $image, $mtimediff);

?>
