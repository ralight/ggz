GGZCONF = $(shell ggz-config -c)

PG_CPPFLAGS = -DWITH_ICU
SHLIB_LINK = -licui18n

MODULE_big = canonicalstr_proc_pgsql
OBJS = canonicalstr_proc_pgsql.o

PGXS := $(shell pg_config --pgxs)
#include $(PGXS)
# FIXME: taken out for snapshot-0.99.1 because doesn't seem to be avilable in Debian at the moment

distdir:

