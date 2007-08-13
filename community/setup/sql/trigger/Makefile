GGZCONF = $(shell ggz-config -c)

PG_CPPFLAGS = -I $(HOME)/ggz/installation/include -DGGZCONF=\"$(GGZCONF)\"
SHLIB_LINK = -L$(HOME)/ggz/installation/lib -lggz -lpq

MODULE_big = ggzsync_trigger
OBJS = ggzsync_trigger.o

PGXS := $(shell pg_config --pgxs)
include $(PGXS)

