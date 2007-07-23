<?php
  $page_title = "KDE Games Server News";
  $site_root = ".";
  include("header.inc");
?>

<?php
  kde_general_news("news.rdf", 32, false);
?>

<?php
  include("footer.inc");
?>
