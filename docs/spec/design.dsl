<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY dbstyle PUBLIC "-//Norman Walsh//DOCUMENT DocBook HTML Stylesheet//EN" CDATA DSSSL>
]>

<style-sheet>
<style-specification use="docbook">
<style-specification-body>

;; These are some customizations to the standard HTML output produced by the
;; Modular DocBook Stylesheets.
;; I've copied parts of a few functions from the stylesheets so these should
;; be checked occasionally to ensure they are up to date.
;;
;; The last check was with version 1.40 of the stylesheets.
;; It will not work with versions < 1.19 since the $shade-verbatim-attr$
;; function was added then. Versions 1.19 to 1.39 may be OK, if you're lucky!

;;(define %generate-book-toc% #f)

;; If a Chapter has role="no-toc" we don't generate a table of contents.
;; This is useful if a better contents page has been added manually, e.g. for
;; the GTK+ Widgets & Objects page. (But it is a bit of a hack.)
(define ($generate-chapter-toc$)
  (not (equal? (attribute-string (normalize "role") (current-node)) "no-toc")))

(define %chapter-autolabel% 
  ;; Are chapters enumerated?
  #f)

(define %use-id-as-filename% #t)

(define %html-ext% ".html")

(define %shade-verbatim% #t)


(mode book-titlepage-recto-mode
  (element title 
    (make element gi: "TABLE"
	  attributes: (list
		       (list "WIDTH" %gentext-nav-tblwidth%)
		       (list "BORDER" "0")
		       (list "BGCOLOR" "#000000")
		       (list "CELLPADDING" "1")
		       (list "CELLSPACING" "0"))
	  (make element gi: "TR"
		(make element gi: "TH"
		      attributes: (list
				   (list "ALIGN" "center")
				   (list "VALIGN" "center"))
		      (make element gi: "FONT"
			    attributes: (list
					 (list "COLOR" "#FFFFFF")
					 (list "SIZE" "7"))
			    (make element gi: "P"
				  attributes: (list (list "CLASS" (gi)))
				  (make element gi: "A"
					attributes: (list (list "NAME" (element-id)))
					(process-children-trim)))))))))

(define (book-titlepage-separator side)
  (empty-sosofo))

;; I want to change this so that for programlisting we use a blue background,
;; but for example or informalexample we use green.
(define ($shade-verbatim-attr$)
  ;; Attributes used to create a shaded verbatim environment.
  (let* ((wrapper-gi (gi (parent (current-node))))
	 (bgcolor (if (or (equal? wrapper-gi (normalize "example"))
			  (equal? wrapper-gi (normalize "informalexample")))
		      "#E3E3E3"
		      "#D6D6D6")))
    (list
     (list "BORDER" "0")
     (list "BGCOLOR" bgcolor)
     (list "WIDTH" ($table-width$))
     (list "CELLPADDING" "6"))))

;; This overrides the tgroup definition (copied from 1.20, dbtable.dsl).
;; It changes the table background color, cell spacing and cell padding.
(element tgroup
  (let* ((wrapper   (parent (current-node)))
	 (frameattr (attribute-string (normalize "frame") wrapper))
	 (pgwide    (attribute-string (normalize "pgwide") wrapper))
	 (footnotes (select-elements (descendants (current-node)) 
				     (normalize "footnote")))
	 (border (if (equal? frameattr (normalize "none"))
		     '(("BORDER" "0"))
		     '(("BORDER" "1"))))
	 (roleattr (attribute-string (normalize "role") wrapper))

	 ;; FIXME: I thought that I should use normalize ("params") etc. here,
	 ;; but that doesn't work. Anyone know why?
	 (bgcolor (cond ((equal? roleattr "params")
			 '(("BGCOLOR" "#FFD0D0")))
			((equal? roleattr "struct")
			 '(("BGCOLOR" "#FFF0D0")))
			((equal? roleattr "enum")
			 '(("BGCOLOR" "#F0F0D0")))
			(else
			 '(("BGCOLOR" "#FFFFFF")))))

	 (width (if (equal? pgwide "1")
		    (list (list "WIDTH" ($table-width$)))
		    '()))
	 (head (select-elements (children (current-node)) (normalize "thead")))
	 (body (select-elements (children (current-node)) (normalize "tbody")))
	 (feet (select-elements (children (current-node)) (normalize "tfoot"))))
    (make element gi: "TABLE"
	  attributes: (append
		       border
		       width
		       bgcolor
		       '(("CELLSPACING" "0"))
		       '(("CELLPADDING" "4"))
		       (if %cals-table-class%
			   (list (list "CLASS" %cals-table-class%))
			   '()))
	  (process-node-list head)
	  (process-node-list body)
	  (process-node-list feet)
	  (make-table-endnotes))))

;; This overrides the refsect2 definition (copied from 1.20, dbrfntry.dsl).
;; It puts a horizontal rule before each function/struct/... description,
;; except the first one in the refsect1.
(element refsect2
  (make sequence
    (if (first-sibling?)
	(empty-sosofo)
	(make empty-element gi: "HR"))
    ($block-container$)))

;; Override the book declaration, so that we generate a crossreference
;; for the book

(element book 
  (let* ((bookinfo  (select-elements (children (current-node)) (normalize "bookinfo")))
	 (ititle   (select-elements (children bookinfo) (normalize "title")))
	 (title    (if (node-list-empty? ititle)
		       (select-elements (children (current-node)) (normalize "title"))
		       (node-list-first ititle)))
	 (nl       (titlepage-info-elements (current-node) bookinfo))
	 (tsosofo  (with-mode head-title-mode
		     (process-node-list title)))
	 (dedication (select-elements (children (current-node)) (normalize "dedication"))))
    (make sequence
     (html-document 
      tsosofo
      (make element gi: "DIV"
	    attributes: '(("CLASS" "BOOK"))
	    (if %generate-book-titlepage%
		(make sequence
		  (book-titlepage nl 'recto)
		  (book-titlepage nl 'verso))
		(empty-sosofo))
	    
	    (if (node-list-empty? dedication)
		(empty-sosofo)
		(with-mode dedication-page-mode
		  (process-node-list dedication)))
	    
	    (if (not (generate-toc-in-front))
		(process-children)
		(empty-sosofo))
	    
	    (if %generate-book-toc%
		(build-toc (current-node) (toc-depth (current-node)))
		(empty-sosofo))
	    
	    ;;	  (let loop ((gilist %generate-book-lot-list%))
	    ;;	    (if (null? gilist)
	    ;;		(empty-sosofo)
	    ;;		(if (not (node-list-empty? 
	    ;;			  (select-elements (descendants (current-node))
	    ;;					   (car gilist))))
	    ;;		    (make sequence
	    ;;		      (build-lot (current-node) (car gilist))
	    ;;		      (loop (cdr gilist)))
	    ;;		    (loop (cdr gilist)))))
	  
	    (if (generate-toc-in-front)
		(process-children)
		(empty-sosofo))))
     (make entity 
       system-id: "index.sgml"
       (with-mode generate-index-mode
	 (process-children))))))

;; Mode for generating cross references

(define (process-child-elements)
  (process-node-list
   (node-list-map (lambda (snl)
                    (if (equal? (node-property 'class-name snl) 'element)
                        snl
                        (empty-node-list)))
                  (children (current-node)))))

(mode generate-index-mode
  (element anchor
    (if (attribute-string "href" (current-node))
	(empty-sosofo)
	(make formatting-instruction data:
	      (string-append "\less-than-sign;ANCHOR id =\""
			     (attribute-string "id" (current-node))
			     "\" href=\""
			     (href-to (current-node))
			     "\"\greater-than-sign;
"))))

  ;; We also want to be able to link to complete RefEntry.
  (element refentry
    (make sequence
      (make formatting-instruction data:
	    (string-append "\less-than-sign;ANCHOR id =\""
			   (attribute-string "id" (current-node))
			   "\" href=\""
			   (href-to (current-node))
			   "\"\greater-than-sign;
"))
      (process-child-elements)))

  (default
    (process-child-elements)))

;; For hypertext links for which no target is found in the document, we output
;; our own special tag which we use later to resolve cross-document links.
(element link 
  (let* ((target (element-with-id (attribute-string (normalize "linkend")))))
    (if (node-list-empty? target)
      (make element gi: "GTKDOCLINK"
	    attributes: (list
			 (list "HREF" (attribute-string (normalize "linkend"))))
            (process-children))
      (make element gi: "A"
            attributes: (list
                         (list "HREF" (href-to target)))
            (process-children)))))


;; This overrides default-header-nav-tbl-noff (copied from 1.20, dbnavig.dsl).
;; I want 'Home' and 'Up' links at the top of each page, and white text on
;; black.
(define (default-header-nav-tbl-noff elemnode prev next prevsib nextsib)
  (let* ((r1? (nav-banner? elemnode))
	 (r1-sosofo (make element gi: "TR"
			  (make element gi: "TH"
				attributes: (list
					     (list "COLSPAN" "4")
					     (list "ALIGN" "center"))
				(make element gi: "FONT"
				      attributes: (list
						   (list "COLOR" "#FFFFFF")
						   (list "SIZE" "5"))
				      (nav-banner elemnode)))))
	 (r2? (or (not (node-list-empty? prev))
		  (not (node-list-empty? next))
		  (nav-context? elemnode)))
	 (r2-sosofo (make element gi: "TR"
			  (make element gi: "TD"
				attributes: (list
					     (list "WIDTH" "25%")
					     (list "BGCOLOR" "#C00000")
					     (list "ALIGN" "left"))
				(if (node-list-empty? prev)
				    (make entity-ref name: "nbsp")
				    (make element gi: "A"
					  attributes: (list
						       (list "HREF" 
							     (href-to 
							      prev)))
					  (make element gi: "FONT"
						attributes: (list
							     (list "COLOR" "#FFFFFF")
							     (list "SIZE" "3"))
						(make element gi: "B"
						      (gentext-nav-prev prev))))))
			 (make element gi: "TD"
			       attributes: (list
					    (list "WIDTH" "25%")
					    (list "BGCOLOR" "#C0C0C0")
					    (list "ALIGN" "center"))
					  (make element gi: "FONT"
						attributes: (list
							     (list "COLOR" "#FFFFFF")
							     (list "SIZE" "3"))
						(make element gi: "B"
						      (nav-home-link elemnode))))
			 (make element gi: "TD"
			       attributes: (list
					    (list "WIDTH" "25%")
					    (list "BGCOLOR" "#B0B0B0")
					    (list "ALIGN" "center"))
					  (make element gi: "FONT"
						attributes: (list
							     (list "COLOR" "#FFFFFF")
							     (list "SIZE" "3"))
						(make element gi: "B"
						      (if (nav-up? elemnode)
							  (nav-up elemnode)
							  (make entity-ref name: "nbsp")))))
			  (make element gi: "TD"
				attributes: (list
					     (list "WIDTH" "25%")
					     (list "BGCOLOR" "#D0D0D0")
					     (list "ALIGN" "right"))
				(if (node-list-empty? next)
				    (make entity-ref name: "nbsp")
				    (make element gi: "A"
					  attributes: (list
						       (list "HREF" 
							     (href-to
							      next)))
					  (make element gi: "FONT"
						attributes: (list
							     (list "COLOR" "#FFFFFF")
							     (list "SIZE" "3"))
						(make element gi: "B"
						      (gentext-nav-next next)))))))))
    (if (or r1? r2?)
	(make element gi: "DIV"
	      attributes: '(("CLASS" "NAVHEADER"))
	  (make element gi: "TABLE"
		attributes: (list
			     (list "WIDTH" %gentext-nav-tblwidth%)
			     (list "BORDER" "0")
			     (list "BGCOLOR" "#000000")
			     (list "CELLPADDING" "1")
			     (list "CELLSPACING" "0"))
		(if r1? r1-sosofo (empty-sosofo))
		(if r2? r2-sosofo (empty-sosofo))))
	(empty-sosofo))))


;; This overrides default-footer-nav-tbl (copied from 1.20, dbnavig.dsl).
;; It matches the header above.
(define (default-footer-nav-tbl elemnode prev next prevsib nextsib)
  (let ((r1? (or (not (node-list-empty? prev))
		 (not (node-list-empty? next))
		 (nav-home? elemnode)))
	(r2? (or (not (node-list-empty? prev))
		 (not (node-list-empty? next))
		 (nav-up? elemnode)))

	(r1-sosofo (make element gi: "TR"
			  (make element gi: "TD"
				attributes: (list
					     (list "WIDTH" "25%")
					     (list "BGCOLOR" "#C00000")
					     (list "ALIGN" "left"))
				(if (node-list-empty? prev)
				    (make entity-ref name: "nbsp")
				    (make element gi: "A"
					  attributes: (list
						       (list "HREF" 
							     (href-to 
							      prev)))
					  (make element gi: "FONT"
						attributes: (list
							     (list "COLOR" "#FFFFFF")
							     (list "SIZE" "3"))
						(make element gi: "B"
						      (gentext-nav-prev prev))))))
			 (make element gi: "TD"
			       attributes: (list
					    (list "WIDTH" "25%")
					    (list "BGCOLOR" "#0000C0")
					    (list "ALIGN" "center"))
					  (make element gi: "FONT"
						attributes: (list
							     (list "COLOR" "#FFFFFF")
							     (list "SIZE" "3"))
						(make element gi: "B"
						      (nav-home-link elemnode))))
			 (make element gi: "TD"
			       attributes: (list
					    (list "WIDTH" "25%")
					    (list "BGCOLOR" "#00C000")
					    (list "ALIGN" "center"))
					  (make element gi: "FONT"
						attributes: (list
							     (list "COLOR" "#FFFFFF")
							     (list "SIZE" "3"))
						(make element gi: "B"
						      (if (nav-up? elemnode)
							  (nav-up elemnode)
							  (make entity-ref name: "nbsp")))))
			  (make element gi: "TD"
				attributes: (list
					     (list "WIDTH" "25%")
					     (list "BGCOLOR" "#C00000")
					     (list "ALIGN" "right"))
				(if (node-list-empty? next)
				    (make entity-ref name: "nbsp")
				    (make element gi: "A"
					  attributes: (list
						       (list "HREF" 
							     (href-to
							      next)))
					  (make element gi: "FONT"
						attributes: (list
							     (list "COLOR" "#FFFFFF")
							     (list "SIZE" "3"))
						(make element gi: "B"
						      (gentext-nav-next next))))))))

	(r2-sosofo (make element gi: "TR"
			 (make element gi: "TD"
			       attributes: (list
					    (list "COLSPAN" "2")
					    (list "ALIGN" "left"))
			       (if (node-list-empty? prev)
				   (make entity-ref name: "nbsp")
				   (make element gi: "FONT"
					 attributes: (list
						      (list "COLOR" "#FFFFFF")
						      (list "SIZE" "3"))
					 (make element gi: "B"
					       (element-title-sosofo prev)))))
			 (make element gi: "TD"
			       attributes: (list
					    (list "COLSPAN" "2")
					    (list "ALIGN" "right"))
			       (if (node-list-empty? next)
				   (make entity-ref name: "nbsp")
				   (make element gi: "FONT"
					 attributes: (list
						      (list "COLOR" "#FFFFFF")
						      (list "SIZE" "3"))
					 (make element gi: "B"
					       (element-title-sosofo next))))))))
    (if (or r1? r2?)
	(make element gi: "DIV"
	      attributes: '(("CLASS" "NAVFOOTER"))
	      (make empty-element gi: "BR"
		    attributes: (list (list "CLEAR" "all")))
	      (make empty-element gi: "BR")

	      (make element gi: "TABLE"
		    attributes: (list
				 (list "WIDTH" %gentext-nav-tblwidth%)
				 (list "BORDER" "0")
				 (list "BGCOLOR" "#000000")
				 (list "CELLPADDING" "1")
				 (list "CELLSPACING" "0"))
		    (if r1? r1-sosofo (empty-sosofo))
		    (if r2? r2-sosofo (empty-sosofo))))
	(empty-sosofo))))


;; This overrides nav-up (copied from 1.20, dbnavig.dsl).
;; We want to change the size and color of the text.
(define (nav-up elemnode)
  (let ((up (parent elemnode)))
    (if (or (node-list-empty? up)
	    (node-list=? up (sgml-root-element)))
	(make entity-ref name: "nbsp")
	(make element gi: "A"
	      attributes: (list
			   (list "HREF" (href-to up)))
	      (make element gi: "FONT"
		    attributes: (list
				 (list "COLOR" "#FFFFFF")
				 (list "SIZE" "3"))
		    (make element gi: "B"
			  (gentext-nav-up up)))))))

;; This overrides nav-home-link (copied from 1.20, dbnavig.dsl).
;; We want to change the size and color of the text.
(define (nav-home-link elemnode)
  (let ((home (nav-home elemnode)))
    (if (node-list=? elemnode home)
	(make entity-ref name: "nbsp")
	(make element gi: "A"
	      attributes: (list
			   (list "HREF" 
				 (href-to home)))
	      (make element gi: "FONT"
		    attributes: (list
				 (list "COLOR" "#FFFFFF")
				 (list "SIZE" "3"))
		    (make element gi: "B"
			  (gentext-nav-home home)))))))

;; These override 2 functions which return the English text to use for links to
;; previous and next pages. (copied from 1.20, dbl1en.dsl).
(define (gentext-en-nav-prev prev) 
  (make sequence (literal "<<< Previous Page")))
(define (gentext-en-nav-next next)
  (make sequence (literal "Next Page >>>")))


(define ($section-body$)
  (make sequence
    (make empty-element gi: "BR"
	  attributes: (list (list "CLEAR" "all")))
    (make element gi: "DIV"
	  attributes: (list (list "CLASS" (gi)))
	  ($section-separator$)
	  ($section-title$)
	  (process-children))))

</style-specification-body>
</style-specification>
<external-specification id="docbook" document="dbstyle">
</style-sheet>
