SUBDIRS = .

noinst_LTLIBRARIES = \
	libgurumod_simplereturn.la \
	libgurumod_programming.la \
	libgurumod_exec.la

libgurumod_simplereturn_la_SOURCES = simplereturn.c
libgurumod_programming_la_SOURCES = programming.c
libgurumod_exec_la_SOURCES = exec.c

INCLUDES = -I../src


plugin:
	gcc -Wall -shared -o gurumod_simplereturn.so simplereturn.c -I ../src
	gcc -Wall -shared -o gurumod_programming.so programming.c -I ../src
	gcc -Wall -shared -o gurumod_exec.so exec.c -I ../src

pluginclean:
	rm -f *.so

