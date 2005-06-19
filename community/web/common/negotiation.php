<?php
include_once("auth.php");
$lang = Auth::language();
$includetype = "direct"; // direct or indirect

if ($lang) :
	$request = $_SERVER["SCRIPT_NAME"];
	$ending = Locale::ending($request);
	if ($ending == "") :
		if (strstr($request, "php")) :
			$request .= "index.php";
		else :
			$request .= ".php";
		endif;
		$ending = Locale::ending($request);
	endif;
	if ($ending == "php") :
		$r = $_SERVER["DOCUMENT_ROOT"];
		$q = $_SERVER["QUERY_STRING"];
		if ($q) :
			$q = "?$q";
		endif;
		if (file_exists("$r$request.$lang")) :
			if ($includetype == "direct") :
				Locale::detect($lang);
				include("$r$request.$lang");
			else :
				header("Location: $request.$lang$q");
			endif;
			exit;
		endif;
	endif;
	if (($ending == $lang) || ($ending == "php")) :
		Locale::detect($lang);
	else :
		Locale::detect();
	endif;
else :
	Locale::detect();
endif;

?>
