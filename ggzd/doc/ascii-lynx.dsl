<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY docbook.dsl PUBLIC "-//Norman Walsh//DOCUMENT DocBook HTML Stylesheet//EN" CDATA dsssl>
]>
<style-sheet>
<style-specification id="html" use="docbook">
<style-specification-body> 
;;
;;  $Id: ascii-lynx.dsl 55 2000-02-19 17:22:37Z bmh $
;; 
;;  Stylesheet that converts to HTML with the correct parameters to do a
;;  subsequent Lynx -dump on it.
;;

(define nochunks #t)
(define %html-manifest% #f)
(define %html-ext% ".html")
(define %use-id-as-filename% #f)

</style-specification-body>
</style-specification>
<external-specification id="docbook" document="docbook.dsl">
</style-sheet>
