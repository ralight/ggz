<?php

/*
 * File: control.c
 * Author: Roger Light
 * Project: ggzd stats
 * Date: 2007/01/20/
 * Desc: Plot something
 * $Id: allplayers.php 8958 2007-01-20 12:02:32Z oojah $
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

$interval = 1152;
$title = "Logged in players (4 day rolling average)";
$datalimit = 403600;
$roll = 1152;

$players = array(
	"name" => "players",
	"label" => "",
	"file" => "/var/log/ggzd/stats.log",
	"scale" => 1,
	"index" => 1,
	"divby" => "",
	"plot" => true
);

$image = "images/allplayers.png";
$mtimediff = 3600*24;

graph_cached_create(array($players), $interval, $title, $datalimit, $roll, $image, $mtimediff);

?>
