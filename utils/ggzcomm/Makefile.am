SUBDIRS = .

commdir = $(prefix)/bin
comm_SCRIPTS = ggzcommgen.rb

EXTRA_DIST = \
	DESIGN \
	protocol-format.txt \
	helloworld.protocol \
	tictactoe.protocol \
	ccheckers.protocol \
	chess.protocol \
	dots.protocol \
	ggzcards.protocol \
	hastings.protocol \
	muehle.protocol

test:
	$(top_builddir)/ggzcomm/ggzcommgen.rb --protocol=tictactoe --format=c --task=server
	$(top_builddir)/ggzcomm/ggzcommgen.rb --protocol=ggzcards --format=c++ --task=client

