<?xml version= "1.0" standalone="no"?>
<!DOCTYPE html SYSTEM "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
                      

<!--This can probably be deleted. -->
<!--It was what we originally used, but it did not work.-->
<!--
<xsl:stylesheet
   xmlns:xsl= "http://www.w3.org/TR/WD-xsl"
   xmlns:fo="http://www.w3.org/XSL/Format/1.0"
   result-ns="fo" indent-result="yes"> 
-->
<!-- end commented code. -->

<xsl:stylesheet version="1.0" 
xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:math="http://www.w3.org/1998/Math/MathML"
xmlns="http://www.w3.org/1999/xhtml">

<!-- Root Node -->
<xsl:template match="/">
	<xsl:processing-instruction name="xml-stylesheet">
		type="text/css" href="spec.css" 
	</xsl:processing-instruction>

<html>
    <head>
        <title>
		<xsl:for-each select="module">
			<xsl:value-of select="@name"/>
		</xsl:for-each>
        </title>

<!--	Javascript so that answer can appear on click.	-->
<script language="javascript">
function showSolution(id) 
{
    document.getElementById(id).style.display="block";
}
function hideSolution(id) 
{
    document.getElementById(id).style.display="none";
}
</script>
<!--	Added by Sarah 8/17/00	Code by James-->
    </head>	
        <xsl:apply-templates/>
</html>
</xsl:template>


<!-- Header and Body for the Module -->
<xsl:template match="module">
    <body>
 	<h1>	
            <xsl:value-of select="@name"/>
        </h1>	
        <xsl:apply-templates/>
<!-- Print footnotes in order at the bottom -->
      
	<xsl:for-each select="//annotation">
 		<xsl:variable name="foot">
    			<xsl:number level="any"/>
 		</xsl:variable>
		<xsl:if test="$foot='1'">
			<hr/>
		</xsl:if>
		<sup>		
			<a name="FOOT{$foot}"><xsl:value-of select='$foot'/></a>
		</sup>  
		<xsl:apply-templates/>
                <br/>
	</xsl:for-each>
<!-- Through here. 8/14/00 by Sarah  -->
     <hr/>
     <a href='http://cnx.rice.edu/bugreport.html'>Submit a BUG REPORT.</a><br/>
     <a href='http://cnx.rice.edu/comment.html'>Submit a SUGGESTION.</a> 
    </body>
</xsl:template>


<!-- Section Header -->
<xsl:template match="section">
    <h2 id='{@id}'>
     	<xsl:number level='single' count='module/section'/>&nbsp;&nbsp;<xsl:value-of select="@name"/>
    </h2>
    <xsl:apply-templates/>
</xsl:template>


<!-- Subsection Header -->
<xsl:template match="section/section">
    <h3 id='{@id}'>
        <xsl:number level='single'
	count='module/section'/>.<xsl:number level='single'
	count='module/section/section'/>&nbsp;&nbsp;<xsl:value-of select="@name"/>
    </h3>
    <xsl:apply-templates/>
</xsl:template>

<!-- subsubsection header -->
<xsl:template match="section/section/section">
    <h4 id='{@id}'>
        <xsl:number level='single' count='module/section'/>.<xsl:number
	level='single' count='module/section/section'/>.<xsl:number
	level='single' count='module/section/section/section'/>&nbsp;&nbsp;<xsl:value-of select="@name"/>
    </h4>
    <xsl:apply-templates/>
</xsl:template>

<!--  subsubsubsection header-->
<xsl:template match="section/section/section/section">
    <h5 id='{@id}'>
        <xsl:number level='single' count='module/section'/>.<xsl:number
	level='single' count='module/section/section'/>.<xsl:number
	level='single' count='module/section/section/section'/><xsl:number
	level='single'
	count='module/section/section/section/section'/>&nbsp;&nbsp;<xsl:value-of select="@name"/>
    </h5>
    <xsl:apply-templates/>
</xsl:template>

<!-- Emphasized Text -->
<xsl:template match="emphasis">
	<i>
	<xsl:apply-templates/>
	</i>
</xsl:template>


<!-- Paragraph. Formatting in CSS under p.para -->
<xsl:template match="para">
	<p class="para" id='{@id}'>
		<xsl:apply-templates/>
	</p>
</xsl:template>


<!-- Default list -->
<xsl:template match="list">
	<ul id='{@id}'>
	<xsl:for-each select="item">
		<li id='{@id}'>
		<xsl:apply-templates/>
		</li>
	</xsl:for-each>
	</ul>
</xsl:template>


<!-- Numbered lists -->
<xsl:template match="list[@type='enumerated']">
	<ol id='{@id}'>
	<xsl:for-each select="item">
		<li id='{@id}'>
		<xsl:apply-templates/>
		</li>
	</xsl:for-each>
	</ol>
</xsl:template>


<!-- Code -->
<xsl:template match="code">
	<pre>
		<xsl:apply-templates/>
	</pre>
</xsl:template>


<!-- Figure -->
<xsl:template match="figure">	

	<xsl:variable name="eq">
		<xsl:number level="any"/>
	</xsl:variable>

        <xsl:variable name="orient">
		<xsl:value-of select="@orient"/>
	</xsl:variable>	

	<a name="{@id}"/>
	<br/>	
	<center>
		<b>
		<xsl:apply-templates select="title"/>
		</b>
		<br/>	
		<xsl:apply-templates select="mediaobject|table"/>

<!-- Change this when we figure out how to have two different possibilities-->	
		<table align="center">
		<xsl:choose>
		<xsl:when test="$orient='vertical'">
			<xsl:for-each select="subfigure">
			<tr>
				<td>
        			<xsl:apply-templates select="mediaobject|table"/>
				<br/>
				<xsl:apply-templates select="caption"/>
				</td>
			</tr>
			</xsl:for-each>
		</xsl:when>
		<xsl:otherwise>
			<tr>
			<xsl:for-each select="subfigure">
				<td>
				<xsl:apply-templates select="mediaobject|table"/>
				<br/>
				<xsl:apply-templates select="caption"/>
				</td>
			</xsl:for-each>
			</tr>
		</xsl:otherwise>
		</xsl:choose>
		</table>
<!-- end commented code  (7.28.00) -->

	</center>
	<p class="caption">
		<font size="-1">
		 	<b>
			Figure <xsl:value-of select="$eq"/>&nbsp;
			</b> 
		 	<xsl:apply-templates select="caption"/>
		</font>	
	</p>
	<br/>
</xsl:template>


<!-- SubFigure -->
<xsl:template match="subfigure">
	<td>
		<xsl:apply-templates select="mediaobject|table"/>
		<br/>
		<xsl:apply-templates select="caption"/>
	</td>
</xsl:template>


<!--FIX ME  -->
<!--This mediaobject should be dependent on type='image'-->
<!--check me (@type="image")-->
<xsl:template match="mediaobject">
       	<img>
		<xsl:attribute name="src">
			<xsl:value-of select="@src"/>
		</xsl:attribute>
	</img>
</xsl:template>


<!--FIX ME  -->
<!-- This mediaobject should be dependent on type='image'-->
<!--check me (@type="image")-->
<xsl:template match="mediaobject">
       	<img>
		<xsl:attribute name="src">
			<xsl:value-of select="@src"/>
		</xsl:attribute>
	</img>
</xsl:template>

<xsl:template match="title">
	<xsl:apply-templates/>
</xsl:template>

<xsl:template match="caption">
	<xsl:apply-templates/>
</xsl:template>

<!-- math:math puts the math tag in the math namespace -->

<xsl:template match="math:math">
	<xsl:copy-of select="."/>
</xsl:template>

<xsl:template match="equation">
	<xsl:variable name="eq">
		<xsl:number level="any"/>
	</xsl:variable>

<!--do something with the para because eqn can go in paras-->
	<center>
	     <a name="{@id}">	
		<table width="100%"> <tr>
			<td width="30"></td>     
			<td align="center" valign="center">
				<xsl:apply-templates/>
			</td>
			<td width="30" align="right">	
				(<xsl:value-of select="$eq"/>)
			</td>
			</tr>
		</table>
	</a>
	</center>
</xsl:template>



<!-- cnxn can have a target attribute, a module attribute, or both.  But it must have one of them.-->
<xsl:template match="cnxn">

<xsl:choose>
	
	<!--both target and module-->
	<xsl:when test="@target and @module">
		<a href="http://cnx.rice.edu/cgi-bin/showmodule.cgi?id={@module}/#{@target}">
	<xsl:if test="not(string())">*</xsl:if>
	<xsl:apply-templates/></a>			</xsl:when>

	<!--only module and not target-->
	<xsl:when test="not(@target)">
		<a href="http://cnx.rice.edu/cgi-bin/showmodule.cgi?id={@module}"><xsl:apply-templates/></a>
	</xsl:when>

	<!--only target and no module
	This looks for all equations or figures (depending on what the link is to) and counts how many there have been until it finds the tag with an id matching the target (stored as 'referto').-->
	<xsl:otherwise>
	<a href="#{@target}">
	<xsl:if test="not(string())">*</xsl:if>
	<xsl:apply-templates/></a>		
<!--
	<xsl:variable name="referto">
		<xsl:value-of select="@target"/>
	</xsl:variable>	
-->	
	<!--tests all equation ids (stored as 'eqn') to find a match-->
<!--	<xsl:for-each select="//equation">
		<xsl:variable name="eqn">
			<xsl:number level="any"/>
		</xsl:variable>
		<xsl:if test="@id=$referto">
			<xsl:choose>
				<xsl:when test="not(@name)">
				<a href="#{@id}">
				Equation <xsl:value-of select="$eqn"/></a>
				</xsl:when>
				<xsl:otherwise>
				<a href="#{@id}">
-->
<!--	IF YOU DO NOT WANT THE NAME OF THE EQUATION TO SHOW UP REMOVE THIS NEXT LINE . . .	-->
<!--
				<xsl:value-of select="@name"/>
-->
<!--	. . . THROUGH HERE	-->
<!--				(Equation <xsl:value-of select="$eqn"/>)</a>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:if>
	</xsl:for-each>
-->
	<!--tests all figure ids (stored at 'fig') to find a match-->
<!--
	<xsl:for-each select="//figure">
		<xsl:variable name="fig">
			<xsl:number level="any"/>
		</xsl:variable>
		<xsl:if test="@id=$referto">
			<a href="#{@id}">
			Figure <xsl:value-of select="$fig"/></a>
		</xsl:if>
	</xsl:for-each>
-->
	</xsl:otherwise>
	
</xsl:choose>
</xsl:template>


<!--Definition should have template, but this is not it.  I was merely fiddling.  I don't think this even works.-->
<!--
<xsl:template match="definition">
	<font color="blue">
		<xsl:apply-templates/>
	</font>
</xsl:template>
-->

<!--change this to look for the type="teacher"-->
<xsl:template match="annotation">
 <xsl:variable name="foot">
    <xsl:number level="any"/>
 </xsl:variable>
 <sup><a href="#FOOT{$foot}"><xsl:value-of select='$foot'/></a></sup>
</xsl:template>




<!--
This prevents the keywords from printing in the module.
-->
<xsl:template match="keywordlist">
</xsl:template>

<!--	Added to make exercises appear and disappear -->
<!--	The for-each commands are over kill but I could not figure out
how else to do it.  -->
<xsl:template match="exercise">
	<xsl:variable name="ex">
		<xsl:number level="any"/>
	</xsl:variable>
	<xsl:variable name="somesolid">
		      <xsl:for-each select="solution">
		      		      <xsl:value-of select="@id"/>
		      </xsl:for-each>   
	</xsl:variable>
        <div id='{@id}' class="problem" onclick="showSolution('{$somesolid}')">
		<xsl:attribute name="id">
		      <xsl:for-each select="problem">
				    <xsl:value-of select="@id"/>
		      </xsl:for-each>
		</xsl:attribute>
			<b>Question <xsl:value-of select="$ex"/>. </b>
        <i><xsl:apply-templates select="problem"/></i>
	</div>
	<div id='{@id}' class="solution" onclick="hideSolution('{$somesolid}')">
	        <xsl:attribute name="id">
		      <xsl:for-each select="solution">
				    <xsl:value-of select="@id"/>
		      </xsl:for-each>
		</xsl:attribute>
		<b>Answer&nbsp;</b> <xsl:apply-templates select="solution"/>
	</div>
</xsl:template>

<xsl:template match="problem">
	      <xsl:apply-templates/>
</xsl:template>

<xsl:template match="solution">
	       <xsl:apply-templates/>
</xsl:template>

<!--	Added by Sarah on 8/17/00 -->


<xsl:template match="example">
	<hr/>
	<b id='{@id}'>Example</b><br/>
	<xsl:apply-templates/>
	<hr/> 
</xsl:template>


<!--This is an early draft.  It needs major modification.-->
<xsl:template match="table">
<table border="3" align="center">
	<xsl:for-each select="categories">
		<tr>
			<xsl:for-each select="category">
				<td><B><xsl:apply-templates/></B></td>
			</xsl:for-each>
		</tr>
	</xsl:for-each>
	<xsl:for-each select="group">
		<tr>
			<xsl:for-each select="elem">
				<td><xsl:apply-templates/></td>
			</xsl:for-each>
		</tr>
	</xsl:for-each>
</table>
<xsl:apply-templates select="caption"/>
</xsl:template>
<!-- end early draft (7.28.00) -->

<!--	 External link	       -->
<xsl:template match='link'>
	<a>
		<xsl:attribute name='href'>
			<xsl:value-of select="@src"/>
		</xsl:attribute>
		<xsl:apply-templates/>
	</a>
</xsl:template>
<!--	added by Sarah (8-18-00)-->

</xsl:stylesheet>













