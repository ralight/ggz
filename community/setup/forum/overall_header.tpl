<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html dir="{S_CONTENT_DIRECTION}">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<meta http-equiv="Content-Style-Type" content="text/css">
<script language="JavaScript" type="text/JavaScript">
<!--
function MM_swapImgRestore() { //v3.0
  var i,x,a=document.MM_sr; for(i=0;a&&i<a.length&&(x=a[i])&&x.oSrc;i++) x.src=x.oSrc;
}

function MM_preloadImages() { //v3.0
  var d=document; if(d.images){ if(!d.MM_p) d.MM_p=new Array();
    var i,j=d.MM_p.length,a=MM_preloadImages.arguments; for(i=0; i<a.length; i++)
    if (a[i].indexOf("#")!=0){ d.MM_p[j]=new Image; d.MM_p[j++].src=a[i];}}
}

function MM_findObj(n, d) { //v4.01
  var p,i,x;  if(!d) d=document; if((p=n.indexOf("?"))>0&&parent.frames.length) {
    d=parent.frames[n.substring(p+1)].document; n=n.substring(0,p);}
  if(!(x=d[n])&&d.all) x=d.all[n]; for (i=0;!x&&i<d.forms.length;i++) x=d.forms[i][n];
  for(i=0;!x&&d.layers&&i<d.layers.length;i++) x=MM_findObj(n,d.layers[i].document);
  if(!x && d.getElementById) x=d.getElementById(n); return x;
}

function MM_swapImage() { //v3.0
  var i,j=0,x,a=MM_swapImage.arguments; document.MM_sr=new Array; for(i=0;i<(a.length-2);i+=3)
   if ((x=MM_findObj(a[i]))!=null){document.MM_sr[j++]=x; if(!x.oSrc) x.oSrc=x.src; x.src=a[i+2];}
}
//-->
</script>
{META} {NAV_LINKS} 
<title>{SITENAME} :: {PAGE_TITLE}</title>
<!-- link rel="stylesheet" href="templates/Chrome_ex/{T_HEAD_STYLESHEET}" type="text/css" -->
<style type="text/css">
<!--
/*
  The original Chrome_ex Theme for phpBB version 2+
  Created by Pigne
  http://www.pigne.net
*/


 /* General page style. The scroll bar colours only visible in IE5.5+ */
body {
	background-color: #E5E5E5;
	scrollbar-face-color: #DEE3E7;
	scrollbar-highlight-color: #FFFFFF;
	scrollbar-shadow-color: #DEE3E7;
	scrollbar-3dlight-color: #D1D7DC;
	scrollbar-arrow-color:  #006699;
	scrollbar-track-color: #EFEFEF;
	scrollbar-darkshadow-color: #98AAB1;
	margin-top: 0px;
	margin-bottom: 0px;
	margin-right: 0px;
	margin-left: 0px;
}

/* General font families for common tags */
font,th,td,p { font-family: Verdana, Arial, Helvetica, sans-serif }
a:link,a:active,a:visited { color : #688CA3; }
a:hover		{ text-decoration: underline; color : #688CA3; }
hr	{ height: 0px; border: solid #688CA 0px; border-top-width: 1px;}


/* This is the border line & background colour round the entire page */
.bodyline	{ background-color: #EDEDED; border: 1px #EDEDED solid; }

/* This is the outline round the main forum tables */
.forumline	{ background-color: #BEC5CD; border: 0px #006699 solid; }


/* Main table cell colours and backgrounds */
td.row1	{ background-color: #EDEDED; }
td.row2	{ background-color: #EDEDED; }
td.row3	{ background-color: #EDEDED; }


/*
  This is for the table cell above the Topics, Post & Last posts on the index.php page
  By default this is the fading out gradiated silver background.
  However, you could replace this with a bitmap specific for each forum
*/
td.rowpic {
		background-color: #BEC5CD;
		background-image: url(templates/Chrome_ex/images/cellpic2.jpg);
		background-repeat: repeat-y;
}

/* Header cells - the blue and silver gradient backgrounds */
th	{
	color: #688CA3; font-size: 10px; font-weight : bold;
	background-color: #BEC5CD; height: 24px;
	background-image: url(templates/Chrome_ex/images/cellpic3.gif);
}

td.cat,td.catHead,td.catSides,td.catLeft,td.catRight,td.catBottom {
			background-image: url(templates/Chrome_ex/images/cellpic1.gif);
			background-color:#BEC5CD; border: #F5F5F5; border-style: solid; height: 24px;
}


/*
  Setting additional nice inner borders for the main table cells.
  The names indicate which sides the border will be on.
  Don't worry if you don't understand this, just ignore it :-)
*/
td.cat,td.catHead,td.catBottom {
	height: 24px;
	border-width: 0px 0px 0px 0px;
}
th.thHead,th.thSides,th.thTop,th.thLeft,th.thRight,th.thBottom,th.thCornerL,th.thCornerR {
	font-weight: bold; border: #EDEDED; border-style: solid; height: 24px; }
td.row3Right,td.spaceRow {
	background-color: #EDEDED; border: #F5F5F5; border-style: solid; }

th.thHead,td.catHead { font-size: 10px; border-width: 1px 1px 0px 1px; }
th.thSides,td.catSides,td.spaceRow	 { border-width: 0px 1px 0px 1px; }
th.thRight,td.catRight,td.row3Right	 { border-width: 0px 1px 0px 0px; }
th.thLeft,td.catLeft	  { border-width: 0px 0px 0px 1px; }
th.thBottom,td.catBottom  { border-width: 0px 1px 1px 1px; }
th.thTop	 { border-width: 1px 0px 0px 0px; }
th.thCornerL { border-width: 1px 0px 0px 1px; }
th.thCornerR { border-width: 1px 1px 0px 0px; }


/* The largest text used in the index page title and toptic title etc. */
.maintitle,h1,h2	{
			font-weight: bold; font-size: 22px; font-family: "Trebuchet MS",Verdana, Arial, Helvetica, sans-serif;
			text-decoration: none; line-height : 120%; color : #688CA3;
}


/* General text */
.gen { font-size : 10px; }
.genmed { font-size : 10px; }
.gensmall { font-size : 10px; }
.gen,.genmed,.gensmall { color : #688CA3; }
a.gen,a.genmed,a.gensmall { color: #688CA3; text-decoration: none; }
a.gen:hover,a.genmed:hover,a.gensmall:hover	{ color: #688CA3; text-decoration: underline; }


/* The register, login, search etc links at the top of the page */
.mainmenu		{ font-size : 10px; color : #688CA3 }
a.mainmenu		{ text-decoration: none; color : #006699;  }
a.mainmenu:hover{ text-decoration: underline; color : #688CA3; }


/* Forum category titles */
.cattitle		{ font-weight: bold; font-size: 10px ; letter-spacing: 1px; color : #FFFFFF}
a.cattitle		{ text-decoration: none; color : #FFFFFF; }
a.cattitle:hover{ text-decoration: underline; }


/* Forum title: Text and link to the forums used in: index.php */
.forumlink		{ font-weight: bold; font-size: 10px; color : #688CA3; }
a.forumlink 	{ text-decoration: none; color : #688CA3; }
a.forumlink:hover{ text-decoration: underline; color : #688CA3; }


/* Used for the navigation text, (Page 1,2,3 etc) and the navigation bar when in a forum */
.nav			{ font-weight: bold; font-size: 11px; color : #688CA3;}
a.nav			{ text-decoration: none; color : #688CA3; }
a.nav:hover		{ text-decoration: underline; }


/* titles for the topics: could specify viewed link colour too */
.topictitle			{ font-weight: bold; font-size: 10px; color : #688CA3; }
a.topictitle:link   { text-decoration: none; color : #688CA3; }
a.topictitle:visited { text-decoration: none; color : #688CA3; }
a.topictitle:hover	{ text-decoration: underline; color : #688CA3; }


/* Name of poster in viewmsg.php and viewtopic.php and other places */
.name			{ font-size : 10px; color : #688CA3;}

/* Location, number of posts, post date etc */
.postdetails		{ font-size : 10px; color : #688CA3; }


/* The content of the posts (body of text) */
.postbody { font-size : 10px;}
a.postlink:link	{ text-decoration: none; color : #688CA3 }
a.postlink:visited { text-decoration: none; color : #688CA3; }
a.postlink:hover { text-decoration: underline; color : #688CA3}


/* Quote & Code blocks */
.code {
	font-family: Courier, 'Courier New', sans-serif; font-size: 10px; color: #688CA3;
	background-color: #FAFAFA; border: #D1D7DC; border-style: solid;
	border-left-width: 1px; border-top-width: 1px; border-right-width: 1px; border-bottom-width: 1px
}

.quote {
	font-family: Verdana, Arial, Helvetica, sans-serif; font-size: 10px; color: #688CA3; line-height: 125%;
	background-color: #FAFAFA; border: #D1D7DC; border-style: solid;
	border-left-width: 1px; border-top-width: 1px; border-right-width: 1px; border-bottom-width: 1px
}


/* Copyright and bottom info */
.copyright		{ font-size: 10px; font-family: Verdana, Arial, Helvetica, sans-serif; color: #688CA3; letter-spacing: -1px;}
a.copyright		{ color: #688CA3; text-decoration: none;}
a.copyright:hover { color: #688CA3; text-decoration: underline;}


/* Form elements */
input,textarea, select {
	color : #688CA3;
	font: normal 11px Verdana, Arial, Helvetica, sans-serif;
	border-color : #EDEDED;
}

/* The text input fields background colour */
input.post, textarea.post, select {
	background-color : #EDEDED;
}

input { text-indent : 2px; }

/* The buttons used for bbCode styling in message post */
input.button {
	background-color : #EFEFEF;
	color : #000000;
	font-size: 11px; font-family: Verdana, Arial, Helvetica, sans-serif;
}

/* The main submit button option */
input.mainoption {
	background-color : #FAFAFA;
	font-weight : bold;
}

/* None-bold submit button */
input.liteoption {
	background-color : #FAFAFA;
	font-weight : normal;
}

/* This is the line in the posting page which shows the rollover
  help line. This is actually a text box, but if set to be the same
  colour as the background no one will know ;)
*/
.helpline { background-color: #DEE3E7; border-style: none; }


/* Import the fancy styles for IE only (NS4.x doesn't use the @import function) */
@import url("formIE.css");
.toptop {
	background-image: url(templates/Chrome_ex/images/toptop.gif);
}
.nabg {
	background-image: url(templates/Chrome_ex/images/lang_english/btn_end.gif);
}
.topbannerbg {
	background-image: url(templates/Chrome_ex/images/topbg.gif);
}
-->
</style>
<!-- BEGIN switch_enable_pm_popup -->
<script language="Javascript" type="text/javascript">
<!--
	if ( {PRIVATE_MESSAGE_NEW_FLAG} )
	{
		window.open('{U_PRIVATEMSGS_POPUP}', '_phpbbprivmsg', 'HEIGHT=225,resizable=yes,WIDTH=400');;
	}
//-->
</script>
<!-- END switch_enable_pm_popup --></head>
<body bgcolor="{T_BODY_BGCOLOR}" text="{T_BODY_TEXT}" link="{T_BODY_LINK}" vlink="{T_BODY_VLINK}" onLoad="MM_preloadImages('templates/Chrome_ex/images/lang_english/btn_register2.gif','templates/Chrome_ex/images/lang_english/btn_index2.gif','templates/Chrome_ex/images/lang_english/btn_profile2.gif','templates/Chrome_ex/images/lang_english/btn_pm2.gif','templates/Chrome_ex/images/lang_english/btn_faq2.gif','templates/Chrome_ex/images/lang_english/btn_users2.gif','templates/Chrome_ex/images/lang_english/btn_search2.gif','templates/Chrome_ex/images/lang_english/btn_groups2.gif','templates/Chrome_ex/images/lang_english/mainsite2.gif')"> 
	<table width="100%" align="center" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td><table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr> 
    <td width="100%" class="toptop">&nbsp;</td>
  </tr>
  <tr> 
    <td width="100%" class="topbannerbg"><a href="{U_INDEX}"><img src="templates/Chrome_ex/images/logo_phpBB.gif" alt="" width="670" height="83" border="0"></a></td>
  </tr>
</table>
<tr> 
		<td height="25" class="bodyline"><table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="nabg">
      <tr align="left" valign="top" class="nabg"> 
        <td width="84"><a href="{U_INDEX}"><img src="templates/Chrome_ex/images/lang_english/btn_index.gif" alt="" width="84" height="27" border="0"></a></td>
            <!-- BEGIN switch_user_logged_out -->
            
        <td width="84"><a href="{U_REGISTER}"><img src="templates/Chrome_ex/images/lang_english/btn_register.gif" alt="" width="84" height="27" border="0"></a></td>
            <!-- END switch_user_logged_out -->
            <!-- BEGIN switch_user_logged_in -->
            
        <td width="84"><a href="{U_PROFILE}"><img src="templates/Chrome_ex/images/lang_english/btn_profile.gif" alt="" width="84" height="27" border="0"></a></td>
            
        <td width="84"><a href="{U_PRIVATEMSGS}"><img src="templates/Chrome_ex/images/lang_english/btn_pm.gif" alt="" width="84" height="27" border="0"></a></td>
            <!-- END switch_user_logged_in -->
            
        <td width="84"><a href="{U_FAQ}"><img src="templates/Chrome_ex/images/lang_english/btn_faq.gif" alt="" width="84" height="27" border="0"></a></td>
            
        <td width="84"><a href="{U_MEMBERLIST}"><img src="templates/Chrome_ex/images/lang_english/btn_users.gif" alt="" width="84" height="27" border="0"></a></td>
            
        <td width="84"><a href="{U_SEARCH}"><img src="templates/Chrome_ex/images/lang_english/btn_search.gif" alt="" width="84" height="27" border="0"></a></td>
            <!-- BEGIN switch_user_logged_in -->
            
        <td width="84"><a href="{U_GROUP_CP}"><img src="templates/Chrome_ex/images/lang_english/btn_groups.gif" alt="" width="84" height="27" border="0"></a></td>
            <td width="134"><a href="{U_LOGIN_LOGOUT}"><img src="templates/Chrome_ex/images/lang_english/btn_logout.gif" alt="" width="84" height="27"></a></td>
            <td width="100%">&nbsp;</td>
            <!-- END switch_user_logged_in -->
            <!-- BEGIN switch_user_logged_out -->
            <form method="post" action="{S_LOGIN_ACTION}">
              <input class="text" type="hidden" name="autologin" value="1" />
            </form>
            <!-- END switch_user_logged_out -->
            <!-- BEGIN switch_user_logged_in -->
            <form action="{S_LOGIN_ACTION}" method="get">
              <input type="hidden" name="logout" value="true" />
      {SID_POST}
            </form>
            <!-- END switch_user_logged_in -->
          </tr>
        </table>		  
		
