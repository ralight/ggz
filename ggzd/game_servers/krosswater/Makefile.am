SUBDIRS = stackpath zoneserver

bin_PROGRAMS = krosswater_server

INCLUDES = -I stackpath -I zoneserver

krosswater_server_SOURCES = main.cpp krosswater_server.cpp

krosswater_server_LDADD = zoneserver/libzoneserver.la stackpath/libstackpath.a -leasysock
