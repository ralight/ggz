<?php

// =============================================================
// GGZ Community Web Services API
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//
// For a permanent reference installation, please refer to
// http://api.ggzcommunity.org/
// =============================================================

// -------------------------------------------------------------
// This file uses the 'ggztop' tool to return an XML-formatted list
// of rooms as well as the player and table count for each.

$statsrt = @system("ggztop -x 2>/dev/null");

if ($statsrt) :
	echo $statsrt;
else:
	echo "<error/>";
endif;

?>
