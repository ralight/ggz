#EXTRA_DIST = vencedor.pot de.po

CATALOG = vencedor
POSOURCES = ../*.cpp ../../kggzlib/*.cpp
POFILES = de.po

XGETTEXT = xgettext
MSGMERGE = msgmerge

all:
	@echo "call 'make messages' explicity to extract translations"

messages:
	@echo "updating catalog $(CATALOG)"
	@$(XGETTEXT) -ki18n -kN_ $(POFLAGS) $(POSOURCES) -o $(CATALOG).pot;
	@for j in $(POFILES); do \
		echo "process $$j"; \
		$(MSGMERGE) $$j $(CATALOG).pot > .$$j 2>/dev/null; \
		mv .$$j $$j; \
	done

