<?php

/*
 * File: control.c
 * Author: Roger Light
 * Project: ggzd stats
 * Date: 2007/01/20/
 * Desc: Plot something
 * $Id: tables.php 8958 2007-01-20 12:02:32Z oojah $
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
$title = "Tables";
$datalimit = 4036;
$roll = 0;

$open = array(
	"name" => "open",
	"label" => "Open",
	"file" => "/var/log/ggzd/stats.log",
	"scale" => 1,
	"index" => 6,
	"divby" => "",
	"plot" => true
);

$created = array(
	"name" => "created",
	"label" => "Created",
	"file" => "/var/log/ggzd/stats.log",
	"scale" => 1,
	"index" => 7,
	"divby" => "",
	"plot" => true
);

$closed = array(
	"name" => "closed",
	"label" => "Closed",
	"file" => "/var/log/ggzd/stats.log",
	"scale" => 1,
	"index" => 8,
	"divby" => "",
	"plot" => true
);

$image = "images/tables.png";
$mtimediff = 3600;

graph_cached_create(array($open, $created, $closed), $interval, $title, $datalimit, $roll, $image, $mtimediff);

?>
