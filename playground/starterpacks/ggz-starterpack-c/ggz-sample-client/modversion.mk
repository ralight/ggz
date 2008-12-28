modversion.h: module.dsc
	rm -f version.h.tmp
	@echo "/* Auto-generated from module.dsc */" \
		>> version.h.tmp
	@grep ^ProtocolEngine module.dsc | \
		sed 's/.*= \(.*\)/#define PROTOCOL_ENGINE "\1"/' \
		>> version.h.tmp
	@grep ^ProtocolVersion module.dsc | \
		sed 's/.*= \(.*\)/#define PROTOCOL_VERSION "\1"/' \
		>> version.h.tmp
	@grep ^Version module.dsc | \
		sed 's/.*= \(.*\)/#define GAME_VERSION "\1"/' \
		>> version.h.tmp
	mv version.h.tmp modversion.h

BUILT_SOURCES = modversion.h
CLEANFILES = modversion.h

