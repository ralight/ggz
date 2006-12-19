PG_CPPFLAGS = -I /home/spillner/ggz/installation/include
SHLIB_LINK = -L/home/spillner/ggz/installation/lib -lggz -lpq

MODULE_big = ggzsync_trigger
OBJS = ggzsync_trigger.o

PGXS := $(shell pg_config --pgxs)
include $(PGXS)

