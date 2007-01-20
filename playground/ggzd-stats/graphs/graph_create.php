<?php

/*
 * File: graph_create.php
 * Project: ggz server stats
 * Author: Roger Light
 * Date: 2007/01/20
 * Desc: Flexible line graph creation
 * $Id: graph_create.php 8958 2007-01-20 12:02:32Z oojah $
 *
 * Copyright (C) 2007 Roger Light.
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

/* 

This file contains functions to produce line graphs of data from different files.
Each file can have multiple columns - although the columns must be separated by
a space not a comma or anything else.

Raw or rolling averages are also possible, as are dividing one column by another.
This isn't the most efficient function, so note that using multiple rolling averages
or divisions will mostly like mean the script will exceed the maximum run time.

There are three functions, all will broadly similar inputs:

graph_cached_create($infoarray, $interval, $title, $datalimit, $roll, $image, $mtimediff)
graph_create_test($infoarray, $interval, $title, $datalimit, $roll, $image, $mtimediff)
graph_create($infoarray, $interval, $title, $datalimit, $roll)

Descriptions
------------

graph_create is the most important, being the one that does all of the work. It
returns an image handle.
graph_cached_create sends an image to the user's browser. If the image is too old
it is regenerated, otherwise the previously generated image is sent for a great
reduction in run time.
graph_create_test simply shows the amount of real (not cpu) time that graph_create
takes to run and should only really be used when modifying graph_create.

Arguments
---------

$infoarray contains the information on what data to plot, and how to plot it. It
does not contain any actual data to be plotted, apart from labels.

For each column that you want to make use of, define an array as follows:

$bw = array(
	"name" => "bandwidth",
	"label" => "Output Bandwidth",
	"file" => "/path/to/bw/data",
	"index" => 0,
	"scale" => 1,
	"divby" => "",
	"plot" => true
);

name: Used to refer to this column when using divby.
label: This is printed on the graph.
file: The location of the file where the data is.
index: The zero based column number of the data in the file.
scale: All values are *divided* by this number before plotting.
divby: The name of another column that the values of this column will be divided
by.
plot: Whether to plot this column or not. Useful in conjunction with divby.

Whether you have multiple columns or not, the $infoarray argument *must* be an
array of columns, so:

graph_create(array($bw), ...
graph_create(array($bw, $players), ...

The divby parameter is processed for each column in the order that they are 
specified in this array, so multiple divbys are possible with planning.

$interval specifies the interval at which a vertical grey line should be drawn on
the graph.

$title is the title of the graph to be printed on it.

$datalimit the maximum number of data points to consider from the file. If the 
number of points exceeds this number, the most recent (furthest down in the file
are used).

$roll defines the number of points to use in a rolling average. Use 0 to disable.

$image is the file location that a .png file will be written to.

$mtimediff gives the time, in seconds, that must have passed since the .png file
was last generated before it will be regenerated.


To Do
-----

* Label scaling (so bw/1000 without losing the resolution)
* Choose image width/height
* 
graph_create(array(of column data info), $interval, $title, $datalimit, $roll)

*/

function graph_cached_create($infoarray, $interval, $title, $datalimit, $roll, $image, $mtimediff)
{
	if(!file_exists($image) || time() - filemtime($image) > $mtimediff) {
		$graph = graph_create($infoarray, $interval, $title, $datalimit, $roll);
		imagepng($graph, $image);
	}else{
		$graph = imagecreatefrompng($image);
	}

	header("Content-type: image/png");
	imagepng($graph);
	imagedestroy($graph);
}

function graph_create_test($infoarray, $interval, $title, $datalimit, $roll, $image, $mtimediff)
{
	$starttime = microtime(true);
	$graph = graph_create($infoarray, $interval, $title, $datalimit, $roll);
	$res = round(microtime(true)-$starttime, 2);
	print("$res seconds");

}

function graph_create($infoarray, $interval, $title, $datalimit, $roll)
{
	if(!is_array($infoarray) || !is_array($infoarray[0])) return;

	$columns = count($infoarray);
	$longest = $shortest = -1;
	for($i = 0; $i < $columns; $i++){
		$data = file($infoarray[$i]["file"]);
		if(count($data) > $datalimit){ // Get $datalimit most recent pieces of data
			$data = array_slice($data, count($data)-$datalimit, $datalimit);
		}
		foreach($data as $d){
			$datax = explode(" ", $d);
			$values[$i][] = ($datax[$infoarray[$i]["index"]]/$infoarray[$i]["scale"]);
		}

		if(count($values[$i]) > $longest || $longest == -1) $longest = count($values[$i]);
		if(count($values[$i]) < $shortest || $shortest == -1) $shortest = count($values[$i]);
	}
	
	if($longest > $shortest){
		// Need to get most same length but most recent for all of them
		for($i = 0; $i < $columns; $i++){
			if(count($values[$i]) > $shortest){
				$values[$i] = array_slice($values[$i], count($values[$i])-$shortest, $shortest);
			}
		}
	}

	$datasize = $shortest - $roll; 
	$imgwidth = 800;
	$imgheight = 300;
	$ox = 30;
	$oy = 0;

	if($datasize<$imgwidth-$ox){
		$limit = $datasize;
		//$xdivby = 1;
		$xdivby = $datasize/($imgwidth-$ox);
		$imgwidth = 837;
		//$imgwidth = $datasize+$ox;
		//$xdivby = ($imgwidth-$ox)/$datasize;
		//$xdivby = $datasize/($imgwidth-$ox);
	}else{
		$xdivby = 2;
		while($datasize/$xdivby > $imgwidth-$ox){
			$xdivby++;
		}
		$imgwidth = round($datasize/$xdivby)+$ox;
		$limit = $datasize;
	}

	for($i = 0; $i < $columns; $i++){
		if(strlen($infoarray[$i]["divby"])){
			$divname = $infoarray[$i]["divby"];
			$found = false;
			for($j = 0; $j < $columns; $j++){
				if($infoarray[$j]["name"] == $divname){
					$found = true;
					$divcol = $j;
					break;
				}
			}
			if(!$found) continue;

			for($j = 0; $j < $shortest; $j++){
				if($values[$divcol][$j]){
					$values[$i][$j] /= $values[$divcol][$j];
				}else{
					$values[$i][$j] = 0;
				}
			}
		}
	}

	/* Rolling average */
	if($roll > 0){
		for($i = 0; $i < $columns; $i++){
			if(!$infoarray[$i]["plot"]) continue;

			for($j = 0; $j < $roll; $j++){
				$mavg[$j] = $values[$i][$j];
			}

			$roll_index = 0;
			for($j = $roll; $j < count($values[$i]); $j++){
				$nvalues[] = array_sum($mavg)/$roll;
				$mavg[$roll_index] = $values[$i][$j];
				$roll_index = ($roll_index+1)%$roll;
			}
			$values[$i] = $nvalues;
		}
	}

	/* Find limits */
	$vmax = $vmin = -1;
	for($i = 0; $i < $columns; $i++){
		if($infoarray[$i]["plot"]){
			if(max($values[$i]) > $vmax || $vmax == -1) $vmax = max($values[$i]);
			if(min($values[$i]) < $vmin || $vmin == -1) $vmin = min($values[$i]);
		}
	}

	$graph = imagecreate($imgwidth, $imgheight);

	$bg = imagecolorallocate($graph, 255, 255, 255);
	$black = imagecolorallocate($graph, 0, 0, 0);
	$grey = imagecolorallocate($graph, 128, 128, 128);
	$lgrey = imagecolorallocate($graph, 192, 192, 192);
	$red = imagecolorallocate($graph, 255, 0, 0);

	$colours[] = imagecolorallocate($graph, 0, 0, 0);
	$colours[] = imagecolorallocate($graph, 255, 0, 0);
	$colours[] = imagecolorallocate($graph, 0, 0, 255);
	$colours[] = imagecolorallocate($graph, 0, 255, 0);

	do_y_intervals($graph, $imgheight, $imgwidth, $ox, $oy, $lgrey);
	do_intervals($graph, $imgheight, $ox, $interval, $limit, $xdivby, $grey);
	//do_intervals($graph, $imgheight, $ox, $interval, $imagewidth, $xdivby, $grey);

	$labelindex = 0;
	for($i = $columns - 1; $i >= 0; $i--){
		if($infoarray[$i]["plot"]){
			do_plot($graph, $imgheight, $colours[$i%count($colours)], $ox, $oy, $xdivby, $limit, $values[$i], $vmax, $vmin);
			if(strlen($infoarray[$i]["label"])){
				imagestring($graph, 2, 35, 15+10*$labelindex, $infoarray[$i]["label"], $colours[$i%count($colours)]);
				$labelindex++;
			}
		}
	}

	do_text($graph, $imgheight, $black, $title, $vmax, $vmin);
	do_axes($graph, $imgheight, $imgwidth, $ox, $oy, $black);

	return $graph;
}

/* Plot axes */
function do_axes($graph, $imgheight, $imgwidth, $ox, $oy, $colour)
{
	imageline($graph, 20, $imgheight-20, $imgwidth, $imgheight-20, $colour);
	imageline($graph, $ox, 0, $ox, $imgheight-10, $colour);
}


function do_text($graph, $imgheight, $colour, $title, $vmax, $vmin)
{
	$number = 5;
	$inc = ($vmax - $vmin) / $number;
	$hinc = ($imgheight-17) / $number;
	imagestring($graph, 2, 0, $imgheight-17, trim(round($vmin, 1)), $colour);
	imagestring($graph, 2, 0, 0, trim(round($vmax, 1)), $colour);
	for($i = 1; $i < $number; $i++){
		imagestring($graph, 2, 0, $hinc*$i, trim(round($vmax - $inc*$i, 1)), $colour);
	}
	imagestring($graph, 2, 35, 0, $title, $colour);
}

/* Plot a data set */
function do_plot($graph, $imgheight, $colour, $ox, $oy, $xdivby, $limit, $values, $vmax, $vmin)
{
	$oldx = $ox;
	$oldy = 0;
	$dtotal = $values[0];
	for($i=0; $i<$limit; $i++){
		$newx = $i/$xdivby + $ox;
		$newy = ($imgheight-20) - (($imgheight-20)/($vmax-$vmin))*(($values[$i])-$vmin);
		imageline($graph, $oldx, $oldy, $newx, $newy, $colour);

		//imageline($graph, $oldx, ($imgheight-20) - (($imgheight-20)/($vmax-$vmin))*($vtotal/($i+1) - $vmin), $newx, ($imgheight-20) - (($imgheight-20)/($vmax-$vmin))*(($vtotal+$values[$i])/($i+2)-$vmin), $red);
		$vtotal += $values[$i];
		$oldx = $newx;
		$oldy = $newy;
	}

}

/* Print vertical interval lines */
function do_intervals($graph, $imgheight, $ox, $interval, $limit, $xdivby, $colour)
{
	for($i = 1; $i < $limit; $i++){
		if($i % round($interval/$xdivby) == 0){ // 24*7*2 = 336
			imageline($graph, $ox+$i, 0, $ox+$i, $imgheight-1, $colour);
		}
	}

}

/* Print horizontal interval lines */
function do_y_intervals($graph, $imgheight, $imgwidth, $ox, $oy, $colour)
{
	$number = 5;
	$oy = 20;
	$gap = ($imgheight - $oy)/$number;
	for($i = 0; $i < $number; $i++){
		imageline($graph, $ox, $gap*$i , $imgwidth, $gap*$i, $colour);
	}
}

?>
