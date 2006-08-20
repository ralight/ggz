<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

$global_leftbar = "disabled";
$global_rightbar = "disabled";
include("top.inc");

?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Java Applet
		<span class="itemleader"> :: </span>
		<a name="ggzjava"></a>
	</h1>
	<div class="text">
<!-- applet integration begin -->

<object
    classid = "clsid:8AD9C840-044E-11D1-B3E9-00805F499D93"
    codebase = "http://java.sun.com/update/1.5.0/jinstall-1_5-windows-i586.cab#Version=5,0,0,5"
    width="640"
    height="480"
    >
    <PARAM NAME = CODE VALUE = "ggz.ui.ClientApplet" >
    <PARAM NAME = ARCHIVE VALUE = "http://live.ggzgamingzone.org/ggz-java-applet.jar" >
    <param name = "type" value = "application/x-java-applet;version=1.5">
    <param name = "scriptable" value = "false">
    <PARAM NAME = "uri" VALUE="ggz://live.ggzgamingzone.org:5688/">

    <comment>
	<embed
            type = "application/x-java-applet;version=1.5" \
            CODE = "ggz.ui.ClientApplet" \
            ARCHIVE = "http://live.ggzgamingzone.org/ggz-java-applet.jar" \
            uri ="ggz://live.ggzgamingzone.org:5688/"
            width="640"
            height="480"
	    scriptable = false
	    pluginspage = "http://java.sun.com/products/plugin/index.html#download">
	    <noembed>
            
            </noembed>
	</embed>
    </comment>
</object>

<!-- applet integration end -->

	</div>
</div>

<?php include("bottom.inc"); ?>
