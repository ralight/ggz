test: all testdir test-ggzd test-games test-msg

testdir:
	-rm -rf $(top_builddir)/test
	-mkdir -p $(top_builddir)/test
	-mkdir -p $(top_builddir)/test/games
	-mkdir -p $(top_builddir)/test/rooms

test-ggzd:
	-cp -f $(top_srcdir)/ggzd/ggzd.motd $(top_builddir)/test
	-cp -f $(top_srcdir)/ggzd/entry.room $(top_builddir)/test/rooms

test-games:
	for i in `find $(top_builddir)/game_servers/ | grep libs`; do \
		if test -x $$i && test -f $$i; then \
			cp $$i $(top_builddir)/test; \
		fi; \
	done
	find $(top_srcdir)/game_servers -name *.dsc \
		-exec cp {} $(top_builddir)/test/games \;
	find $(top_srcdir)/game_servers -name *.room \
		-exec cp {} $(top_builddir)/test/rooms \;
 
test-msg:
	@echo
	@echo "You may now run:"
	@echo "    $(top_builddir)/ggzd/ggzd -F -f $(top_builddir)/ggzd/ggzd.test"
	@echo "to test server"
	@echo

