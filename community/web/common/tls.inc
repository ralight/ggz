	<div class="menubar" style="left:8%;width:100px">
		<div class="menu">
			<?php $uri = $_SERVER["HTTP_HOST"] . $_SERVER["REQUEST_URI"]; ?>
			<a href="<?php echo "https://$uri"; ?>" class="menuitem" title="Secure browing">Use TLS!</a>
		</div>
	</div>
