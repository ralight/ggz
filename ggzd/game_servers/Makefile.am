## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2276 2001-08-27 10:29:46Z jdorje $

# For no particular reason, I put these in alphabetical order
SUBDIRS = libggzmod \
	  tictactoe reversi chess chinese-checkers combat dots \
	  escape ggzcards hastings  krosswater lapocha spades


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
