SUBDIRS = .

lib_LTLIBRARIES = \
	libgurumod_simplereturn.la \
	libgurumod_programming.la \
	libgurumod_exec.la \
	libgurumod_badword.la

libgurumod_simplereturn_la_SOURCES = simplereturn.c

libgurumod_programming_la_SOURCES = programming.c

libgurumod_exec_la_SOURCES = exec.c
libgurumod_exec_la_LIBADD = -lggzcore

libgurumod_badword_la_SOURCES = badword.c
libgurumod_badword_la_LIBADD = -lggzcore

INCLUDES = -I../src


plugin:
	gcc -Wall -shared -o gurumod_simplereturn.so simplereturn.c -I ../src
	gcc -Wall -shared -o gurumod_programming.so programming.c -I ../src
	gcc -Wall -shared -o gurumod_exec.so exec.c -I ../src
	gcc -Wall -shared -o gurumod_programming.so programming.c -I ../src

pluginclean:
	rm -f *.so

test:
	rm -f /usr/local/lib/libgurumod_programming.so
	rm -f /usr/local/lib/libgurumod_simplereturn.so
	rm -f /usr/local/lib/libgurumod_exec.so
