## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4996 2002-10-22 17:29:01Z dr_maux $

# For no particular reason, I put these in alphabetical order
SUBDIRS = tictactoe reversi chess chinese-checkers combat dots \
	ggzcards hastings krosswater lapocha spades muehle \
	escape keepalive geekgame


test:test-recursive

test-recursive:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	dot_seen=no; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    dot_seen=yes; \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) -s $(AM_MAKEFLAGS) $$local_target) \
	done; \
	if test "$$dot_seen" = "no"; then \
	  $(MAKE) -s $(AM_MAKEFLAGS) "$$target-am" || exit 1; \
	fi; test -z "$$fail"

test-am:
