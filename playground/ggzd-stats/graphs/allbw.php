<?php

/*
 * File: allbw.php
 * Author: Roger Light
 * Project: ggzd stats
 * Date: 2007/01/20/
 * Desc: Plot something
 * $Id: allbw.php 8958 2007-01-20 12:02:32Z oojah $
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
$title = "   Output Bandwidth (4 day rolling average)";
$datalimit = 403600;
$roll = 1152;

$bw = array(
	"name" => "Bandwidth",
	"label" => "",
	"file" => "/var/log/ggzd/ggzout.log",
	"scale" => 300,
	"index" => 2,
	"divby" => "",
	"plot" => true
);

$image = "images/allbw.png";
$mtimediff = 3600*24;

graph_cached_create(array($bw), $interval, $title, $datalimit, $roll, $image, $mtimediff);

?>
