	<div class="text">
	The list of players has already grown a lot.
	To narrow down your search, you can apply some filters.
	</div>
	<div class="text">

<form action='playerfinder.php?find=1' method='POST'>
<table border="0">
<tr><td>Gender:</td><td>
	<select name='user_gender'>
<?php
include_once("genderlist.php");
$g = new Gender();
$g->listall();
?>
	</select>
</td></tr>
<tr><td>Country:</td><td>
	<select name='user_country'>
<?php
include_once("countrylist.php");
$c = new Country();
$c->listall();
?>
	</select>
</td></tr>
<tr><td>Status:</td><td>
	<select name='user_status'>
	<option value=''>(undisclosed)</option>
	<option value='registered'>registered user</option>
	<option value='bot'>bot player</option>
	<option value='host'>host player</option>
	<option value='admin'>administrator</option>
	</select>
</td></tr>
<tr><td>Karma:</td><td>
	<select name='user_karma'>
	<option value=''>(undisclosed)</option>
	<option value='good'>good karma (8-10)</option>
	<option value='medium'>medium karma (4-7)</option>
	<option value='bad'>bad karma (0-3)</option>
	</select>
</td></tr>
<tr><td>Veteran:</td><td>
	<select name='user_veteran'>
	<option value=''>(undisclosed)</option>
	<option value='newbie'>Newbie (1-2 weeks)</option>
	<option value='amateur'>Amateur (3-4 weeks)</option>
	<option value='knownplayer'>Known player (1-2 months)</option>
	<option value='longtimeplayer'>Long-time player (2-6 months)</option>
	<option value='veteran'>Veteran (6-12 months)</option>
	<option value='veteranhonours'>Veteran with Honours (&gt; 1 year)</option>
	</select>
</td></tr>
<tr><td></td><td><input type='submit' value='Find'></td></tr>
</table>
</form>

	</div>

