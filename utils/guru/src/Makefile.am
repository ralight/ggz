SUBDIRS = .

noinst_PROGRAMS = guru gi18n

guru_SOURCES = guru.c main.c module.c
guru_LDADD = -ldl

gi18n_SOURCES = i18n.c test_i18n.c
gi18n_LDADD =

noinst_HEADERS = guru.h module.h i18n.h gurumod.h


#all:
#	gcc -Wall -c guru.c
#	gcc -Wall -c main.c
#	gcc -Wall -c module.c
#	gcc -o guru main.o guru.o module.o -ldl
#	gcc -Wall -c i18n.c
#	gcc -Wall -c test_i18n.c
#	gcc -o guru_i18n i18n.o test_i18n.o
#
#clean:
#	rm -f guru
#	rm -f guru_i18n
#	rm -f *.o

