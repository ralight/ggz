<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">

<!--
<!DOCTYPE html PUBLIC
  "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
-->
<html xmlns="http://www.w3.org/1999/xhtml">

<head>
<title>Protocol documentation: <xsl:value-of select="ggzcomm/@engine"/></title>
<style type="text/css">
body {
	background-color: #E4EBC3;
	margin-top: 0px;
	margin-left: 0px;
	margin-right: 0px;
	margin-bottom: 0px;
	font-family: sans-serif;
}
div {
	padding-top: 10px;
	padding-left: 20px;
	padding-right: 20px;
	padding-bottom: 10px;
}
a:link {
	color: #555555;
	font-weight: bold;
}
a:visited {
	color: #000000;
	font-weight: bold;
}
a:hover {
	color: #999999;
	font-weight: bold;
}
a:active {
	color: #333333;
	font-weight: bold;
}
#message {
	color: #800000;
	font-weight: bold;
}
#message-value {
	color: #000000;
	font-weight: bold;
}
#message-data, #message-type {
	color: #000080;
	font-weight: bold;
}
#link-client, #link-server {
	color: #008080;
	font-weight: bold;
}
#sequence {
	color: #d07000;
	font-weight: bold;
}
</style>
</head>

<body>

<div>

<h1>Protocol documentation: <xsl:value-of select="ggzcomm/@engine"/></h1>

<p>
This page has been autogenerated and describes how the <xsl:value-of select="ggzcomm/@engine"/> engine
communication between client and server works.
Download the raw XML protocol specification:
<a>
<xsl:attribute name="href">
<xsl:value-of select="ggzcomm/@engine"/>.protocol
</xsl:attribute>
<xsl:value-of select="ggzcomm/@engine"/>.protocol
</a>
</p>

<h2>
Symbol definitions
</h2>

<p>
<xsl:for-each select="ggzcomm/definitions/def">
<span id="message">
<xsl:value-of select="@name"/>
</span>
=
<span id="message-value">
<xsl:value-of select="@value"/>
</span>
<br />
</xsl:for-each>
</p>

<h2>
Consistency order rules
</h2>

<p>
<xsl:for-each select="ggzcomm/protocol/link">
<span id="link-client">
<xsl:value-of select="@client"/>
</span>
(client) =&gt;
<span id="link-server">
<xsl:value-of select="@server"/>
</span>
(server)<br />
</xsl:for-each>
</p>

<h2>
Messages from the server to the client
</h2>

<p>
<xsl:for-each select="ggzcomm/server/event">
Message:
<span id="message">
<xsl:value-of select="@name"/>
</span>
<br />
<xsl:for-each select="data">
Data:
<span id="message-data">
<xsl:value-of select="@name"/>
</span>
of type
<span id="message-type">
<xsl:value-of select="@type"/>
</span>
<br />
</xsl:for-each>

<xsl:for-each select="eval">
Condition: on
<span id="sequence">
<xsl:value-of select="condition/@name"/>
</span>
, must be
<span id="sequence">
<xsl:value-of select="condition/@type"/>
</span>
to
<span id="sequence">
<xsl:value-of select="condition/@result"/>
</span>
.
<br />
<!-- data-in-eval -->
<xsl:for-each select="data">
<span id="sequence">
(eval)
</span>
Data:
<span id="message-data">
<xsl:value-of select="@name"/>
</span>
of type
<span id="message-type">
<xsl:value-of select="@type"/>
</span>
<br />
</xsl:for-each>
<!-- /data-in-eval -->
</xsl:for-each>

<xsl:for-each select="sequence">
Sequence: loop
<span id="sequence">
<xsl:value-of select="@count"/>
</span>
times.
<br />
<!-- data-in-sequence -->
<xsl:for-each select="data">
<span id="sequence">
(seq)
</span>
Data:
<span id="message-data">
<xsl:value-of select="@name"/>
</span>
of type
<span id="message-type">
<xsl:value-of select="@type"/>
</span>
<br />
</xsl:for-each>
<!-- /data-in-sequence -->

<!-- eval-in-sequence -->
<xsl:for-each select="eval">
<span id="sequence">
(seq)
</span>
Condition: on
<span id="sequence">
<xsl:value-of select="condition/@name"/>
</span>
, must be
<span id="sequence">
<xsl:value-of select="condition/@type"/>
</span>
to
<span id="sequence">
<xsl:value-of select="condition/@result"/>
</span>
.
<br />
<!-- data-in-eval -->
<xsl:for-each select="data">
<span id="sequence">
(seq)(eval)
</span>
Data:
<span id="message-data">
<xsl:value-of select="@name"/>
</span>
of type
<span id="message-type">
<xsl:value-of select="@type"/>
</span>
<br />
</xsl:for-each>
<!-- /data-in-eval -->
</xsl:for-each>
<!-- /eval-in-sequence -->

<!-- sequence-in-sequence -->
<xsl:for-each select="sequence">
<span id="sequence">
(seq)
</span>
Sequence: loop
<span id="sequence">
<xsl:value-of select="@count"/>
</span>
times.
<br />
<!-- data-in-sequence -->
<xsl:for-each select="data">
<span id="sequence">
(seq)(seq)
</span>
Data:
<span id="message-data">
<xsl:value-of select="@name"/>
</span>
of type
<span id="message-type">
<xsl:value-of select="@type"/>
</span>
<br />
</xsl:for-each>
<!-- /data-in-sequence -->
</xsl:for-each>
<!-- /sequence-in-sequence -->
</xsl:for-each>

<br />
</xsl:for-each>
</p>

<h2>
Messages from the client to the server
</h2>

<p>
<xsl:for-each select="ggzcomm/client/event">
Message:
<span id="message">
<xsl:value-of select="@name"/>
</span>
<br />
<xsl:for-each select="data">
Data:
<span id="message-data">
<xsl:value-of select="@name"/>
</span>
of type
<span id="message-type">
<xsl:value-of select="@type"/>
</span>
<br />
</xsl:for-each>
<br />
</xsl:for-each>
</p>

</div>

</body>

</html>

</xsl:template>

</xsl:stylesheet>
