<? include_once($_SERVER['DOCUMENT_ROOT'] . "/site/header.php"); ?>

La p&aacute;gina requerida no se ha encontrado :(
<br>
<?php
if ($_SERVER['HTTP_REFERER']) :
	echo "<a href='", $_SERVER['HTTP_REFERER'], "'>Go back</a>\n";
else :
	echo "<a href='/'>Ir a la p&aacute;gina de inicio</a>\n";
endif;
?>
<br>

<? include_once($_SERVER['DOCUMENT_ROOT'] . "/site/footer.php"); ?>

