<html>
<head>
<title>
GGZ Community Configuration
</title>
<style type="text/css">
table {
border-spacing: 0px;
border-width: 0px;
}
table td {
background-color: #b0b0b0;
padding: 2px;
}
.sel {
background-color: #d0d0d0;
}
.head {
background-color: #ffffff;
border-width: 1px;
border-style: solid;
border-color: #b0b0b0;
}
.headexpl {
background-color: #ffffff;
border-width: 1px;
border-style: solid;
border-color: #b0b0b0;
font-size: smaller;
}
.gap {
background-color: #ffffff;
}
</style>
</head>
<body>

<?php

include("../common/config.php");

if ($_POST["savevalues"]) :
	echo "**save**<br/>\n";

	$f = fopen("/tmp/tmpconf", "w");
	if ($f) :
		fwrite($f, "<?php\n");
		fwrite($f, "// Generated by configuration.php\n");
		if ($_POST["form_database"] == "configfile") :
			fwrite($f, "\$ggzconfigfile = \"" . $_POST["form_ggzconfigfile"] . "\";\n");
		else :
			fwrite($f, "\$dbhost = \"" . $_POST["form_dbhost"] . "\";\n");
			fwrite($f, "\$dbport = \"" . $_POST["form_dbport"] . "\";\n");
			fwrite($f, "\$dbname = \"" . $_POST["form_dbname"] . "\";\n");
			fwrite($f, "\$dbuser = \"" . $_POST["form_dbuser"] . "\";\n");
			fwrite($f, "\$dbpass = \"" . $_POST["form_dbpass"] . "\";\n");
			fwrite($f, "\$dbtype = \"" . $_POST["form_dbtype"] . "\";\n");
		endif;

		if ($_POST["form_ggzserver"] == "query") :
			fwrite($f, "\$ggzurl= \"" . $_POST["form_ggzurl"] . "\";\n");
		else :
			fwrite($f, "\$ggzhost = \"" . $_POST["form_ggzhost"] . "\";\n");
			fwrite($f, "\$ggzname = \"" . $_POST["form_ggzname"] . "\";\n");
			fwrite($f, "\$ggzversion = \"" . $_POST["form_ggzversion"] . "\";\n");
		endif;
		fwrite($f, "?>\n");
	endif;

	$config_object = new Config("/tmp/tmpconf");
endif;

if (Config::getvalue("ggzconfigfile")) :
	$checked_database_configfile = " checked=\"checked\"";
	$checked_database_manual = "";
else :
	$checked_database_configfile = "";
	$checked_database_manual = " checked=\"checked\"";
endif;

if (Config::getvalue("ggzurl")) :
	$checked_ggz_server = " checked=\"checked\"";
	$checked_ggz_manual = "";
else :
	$checked_ggz_server = "";
	$checked_ggz_manual = " checked=\"checked\"";
endif;

$global_problems = array();

function problem($problem)
{
	global $global_problems;

	$global_problems[] = $problem;
}

if ($checked_ggz_server) :
	if (!Config::getvalue("ggzurl")) :
		problem("GGZ URI is not set.");
	else :
		include("../common/ggzd.php");
		$ggzd = new GGZD(Config::getvalue("ggzurl"));
		$ggzd->parse();
		if ($ggzd->error) :
			problem("GGZ URI is set but no connection is possible.");
		endif;
	endif;
endif;
if ($checked_ggz_manual) :
	if (!Config::getvalue("ggzhost")) :
		problem("GGZ hostname is not set");
	endif;
	if (!Config::getvalue("ggzname")) :
		problem("GGZ server name is not set");
	endif;
	if (!Config::getvalue("ggzversion")) :
		problem("GGZ server version is not set");
	endif;
endif;

if (sizeof($global_problems) > 0) :
	foreach ($global_problems as $problem)
	{
		echo "<i>Problem: $problem</i><br/>\n";
	}
	echo "<br/>";
endif;

?>

<form action="configuration.php" method="post">

<table>
<tr>
<td colspan="2" class="head">Database configuration</td>
</tr><tr>
<td colspan="2" class="headexpl">blah blah blah blah blah</td>
</tr><tr>
<td colspan="2" class="sel"><input type="radio" name="form_database" value="configfile"<?php echo $checked_database_configfile; ?>/> Use ggzd configuration file</td>
</tr><tr>
<td>Path to file:</td><td><input type="text" name="form_ggzconfigfile" value="<?php Config::put("ggzconfigfile"); ?>"></td>
</tr><tr>
<td colspan="2" class="sel"><input type="radio" name="form_database" value="manual"<?php echo $checked_database_manual; ?>/> Configure values manually</td>
</tr><tr>
<td>Database type:</td><td><select name="form_dbtype"><option value="postgresql">PostgreSQL</option><option value="mysql">MySQL</option></select></td>
</tr><tr>
<td>Database name:</td><td><input type="text" name="form_dbname" value="<?php Config::put("dbname"); ?>"></td>
</tr><tr>
<td>Hostname:</td><td><input type="text" name="form_dbhost" value="<?php Config::put("dbhost"); ?>"></td>
</tr><tr>
<td>Port:</td><td><input type="text" name="form_dbport" value="<?php Config::put("dbport"); ?>"></td>
</tr><tr>
<td>Username:</td><td><input type="text" name="form_dbuser" value="<?php Config::put("dbuser"); ?>"></td>
</tr><tr>
<td>Password:</td><td><input type="text" name="form_dbpass" value="<?php Config::put("dbpass"); ?>"></td>
</tr><tr>
<td colspan="2" class="gap">&nbsp;</td>
</tr><tr>
<td colspan="2" class="head">GGZ server configuration</td>
</tr><tr>
<td colspan="2" class="headexpl">blah blah blah blah blah</td>
</tr><tr>
<td colspan="2" class="sel"><input type="radio" name="form_ggzserver" value="query"<?php echo $checked_ggz_server; ?>/> Use live query on GGZ server</td>
</tr><tr>
<td>URI of GGZ server:</td><td><input type="text" name="form_ggzurl" value="<?php Config::put("ggzurl"); ?>"></td>
</tr><tr>
<td colspan="2" class="sel"><input type="radio" name="form_ggzserver" value="manual"<?php echo $checked_ggz_manual; ?>/> Configure values manually</td>
</tr><tr>
<td>Hostname:</td><td><input type="text" name="form_ggzhost" value="<?php Config::put("ggzhost"); ?>"></td>
</tr><tr>
<td>Server name:</td><td><input type="text" name="form_ggzname" value="<?php Config::put("ggzname"); ?>"></td>
</tr><tr>
<td>Server version:</td><td><input type="text" name="form_ggzversion" value="<?php Config::put("ggzversion"); ?>"></td>
</tr><tr>
<td colspan="2" class="sel">Miscellaneous</td>
</tr><tr>
<td>Path to savegame directory:</td><td><input type="text" name="form_ggzgamedir" value="<?php Config::put("ggzgamedir"); ?>"></td>
</tr><tr>
<td colspan="2" class="gap">&nbsp;</td>
</tr><tr>
<td colspan="2" class="head">GGZ Community configuration</td>
</tr><tr>
<td colspan="2" class="headexpl">blah blah blah blah blah</td>
</tr><tr>
<td>Name:</td><td><input type="text" name="form_communityname" value="<?php Config::put("communityname"); ?>"></td>
</tr><tr>
<td>Slogan:</td><td><input type="text" name="form_communityslogan" value="<?php Config::put("communityslogan"); ?>"></td>
</tr><tr>
<td>URL:</td><td><input type="text" name="form_communityurl" value="<?php Config::put("communityurl"); ?>"></td>
</tr><tr>
<td>Contact mail address:</td><td><input type="text" name="form_communitymail" value="<?php Config::put("communitymail"); ?>"></td>
</tr><tr>
<td>Copyright title:</td><td><input type="text" name="form_copyright_title" value="<?php Config::put("copyright_title"); ?>"></td>
</tr><tr>
<td>Copyright URL:</td><td><input type="text" name="form_copyright_link" value="<?php Config::put("copyright_link"); ?>"></td>
</tr><tr>
<td colspan="2" class="gap">&nbsp;</td>
</tr><tr>
<td colspan="2" class="head">GGZ Community appearance</td>
</tr><tr>
<td colspan="2" class="headexpl">blah blah blah blah blah</td>
</tr><tr>
<td colspan="2" class="sel">Miscellaneous</td>
</tr><tr>
<td>Advertise TLS:</td><td><input type="checkbox" name="form_communitytls"></td>
</tr><tr>
<td>Theme:</td><td><select name="form_communitytheme"><option value="default">Default</option><option value="freeciv">Freeciv</option><option value="widelands">Widelands</option></select></td>
</tr><tr>
<td colspan="2" class="sel">Custom modules</td>
</tr><tr>
<td>Blog aggregation:</td><td><input type="checkbox" name="form_features_blogs"></td>
</tr><tr>
<td>Integrated forum:</td><td><input type="checkbox" name="form_features_forum"></td>
</tr><tr>
<td>Show articles:</td><td><input type="checkbox" name="form_features_articles"></td>
</tr><tr>
<td>Offer GGZ games:</td><td><input type="checkbox" name="form_features_ggzgames"></td>
</tr><tr>
<td>Offer web games:</td><td><input type="checkbox" name="form_features_webgames"></td>
</tr><tr>
<td>World map:</td><td><input type="checkbox" name="form_features_worldmap"></td>
</tr><tr>
<td>Data repositories:</td><td><input type="checkbox" name="form_features_datarepo"></td>
</tr><tr>
<td>Karma system:</td><td><input type="checkbox" name="form_features_karma"></td>
</tr><tr>
<td colspan="2" class="gap">&nbsp;</td>
</tr><tr>
<td></td><td><input type="submit" name="savevalues" value="Save values"></td>
</tr>
</table>

</form>

</body>
</html>
