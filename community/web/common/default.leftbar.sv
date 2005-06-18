<div id="leftbar">
	<form class="leather" action="/login/" method="post">
		<h1>Logga in</h1>
		<h2>Användarnamn</h2>
		<input type="text" size="10" maxlength="15" name="input_user">
		<h2>Lösenord</h2>
		<input type="password" size="10" maxlength="15" name="input_pass">
		<br>
		<input type="submit" size="10" maxlength="15" value="Login" class="button">
		<br>
		<?php Auth::checklogin(); ?>
		<br>
		<a href="/login/?task=register">Registrera en ny användare</a>
		<br>
		<a href="/login/?task=resend">Förlorat lösenordet?</a>
	</form>
	
	<hr class="leather">

	<h1>Behöver du hjälp?</h1>
	<a href="mailto:<?php Config::put("mail"); ?>" title="Adminmejl">Skicka ett mejl</a>
	till administratörerna...
</div>

