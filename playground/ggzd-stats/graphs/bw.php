<?php

/*
 * File: control.c
 * Author: Roger Light
 * Project: ggzd stats
 * Date: 2007/01/20/
 * Desc: Plot something
 * $Id: bw.php 8958 2007-01-20 12:02:32Z oojah $
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
$title = "Bandwidth (B/s, fortnight max, 1 day interval)";
$datalimit = 4036;
$roll = 0;

$bwin = array(
	"name" => "bwin",
	"label" => "Input",
	"file" => "/var/log/ggzd/ggzin.log",
	"scale" => 300,
	"index" => 2,
	"divby" => "",
	"plot" => true
);

$bwout = array(
	"name" => "bwout",
	"label" => "Output",
	"file" => "/var/log/ggzd/ggzout.log",
	"scale" => 300,
	"index" => 2,
	"divby" => "",
	"plot" => true
);

$image = "images/bw.png";
$mtimediff = 3600;

graph_cached_create(array($bwin, $bwout), $interval, $title, $datalimit, $roll, $image, $mtimediff);

?>
