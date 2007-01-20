<?php

/*
 * File: control.c
 * Author: Roger Light
 * Project: ggzd stats
 * Date: 2007/01/20/
 * Desc: Plot something
 * $Id: mem.php 8958 2007-01-20 12:02:32Z oojah $
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
$title = "Memory usage (KB)";
$datalimit = 4036;
$roll = 0;

$swap = array(
	"name" => "swap",
	"label" => "Swap",
	"file" => "/var/log/ggzd/mem.log",
	"scale" => 1000,
	"index" => 1,
	"divby" => "",
	"plot" => true
);

$free = array(
	"name" => "free",
	"label" => "Free",
	"file" => "/var/log/ggzd/mem.log",
	"scale" => 1000,
	"index" => 2,
	"divby" => "",
	"plot" => true
);

$buffered = array(
	"name" => "buffered",
	"label" => "Buffered",
	"file" => "/var/log/ggzd/mem.log",
	"scale" => 1000,
	"index" => 3,
	"divby" => "",
	"plot" => true
);

$cached = array(
	"name" => "cached",
	"label" => "Cached",
	"file" => "/var/log/ggzd/mem.log",
	"scale" => 1000,
	"index" => 4,
	"divby" => "",
	"plot" => true
);

$image = "images/mem.png";
$mtimediff = 3600;

graph_cached_create(array($swap, $free, $buffered, $cached), $interval, $title, $datalimit, $roll, $image, $mtimediff);

?>
