## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3630 2002-03-23 19:54:26Z dr_maux $

# For no particular reason, I put these in alphabetical order
SUBDIRS = tictactoe reversi chess chinese-checkers combat dots \
	hastings  krosswater lapocha spades muehle

# for 0.0.6 add:
# escape keepalive


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
