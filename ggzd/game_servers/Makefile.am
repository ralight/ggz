## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2273 2001-08-27 06:48:01Z jdorje $

SUBDIRS = libggzmod spades tictactoe reversi combat krosswater \
	  dots chinese-checkers escape lapocha chess hastings
# I removed the following because they're not compatible with ggzdmod:
# ggzcards



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
