<?php

/*
 * File: control.c
 * Author: Roger Light
 * Project: ggzd stats
 * Date: 2007/01/20/
 * Desc: Plot something
 * $Id: players.php 8958 2007-01-20 12:02:32Z oojah $
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
$title = "Players";
$datalimit = 4036;
$roll = 0;

$total = array(
	"name" => "total",
	"label" => "Total",
	"file" => "/var/log/ggzd/stats.log",
	"scale" => 1,
	"index" => 1,
	"divby" => "",
	"plot" => true
);

$anon = array(
	"name" => "anon",
	"label" => "Anon",
	"file" => "/var/log/ggzd/stats.log",
	"scale" => 1,
	"index" => 2,
	"divby" => "",
	"plot" => true
);

$regd = array(
	"name" => "regd",
	"label" => "Regd",
	"file" => "/var/log/ggzd/stats.log",
	"scale" => 1,
	"index" => 3,
	"divby" => "",
	"plot" => true
);

$loggedin = array(
	"name" => "loggedin",
	"label" => "Logged In",
	"file" => "/var/log/ggzd/stats.log",
	"scale" => 1,
	"index" => 4,
	"divby" => "",
	"plot" => true
);

$image = "images/players.png";
$mtimediff = 0;//3600;

graph_cached_create(array($total, $anon, $regd, $loggedin), $interval, $title, $datalimit, $roll, $image, $mtimediff);

?>
