/* acconfig.h
   This file is in the public domain.

   Descriptive text for the C preprocessor macros that
   the distributed Autoconf macros can define.
   No software package will use all of them; autoheader copies the ones
   your configure.in uses into your configuration header file templates.

   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  Although this order
   can split up related entries, it makes it easier to check whether
   a given entry is in the file.

   Leave the following blank line there!!  Autoheader needs it.  */

/* defined by automake */

#undef AF_LOCAL
#undef DEBUG
#undef DEBUG_SOCKET
#undef DEBUG_MEM
#undef GGZDCONFDIR
#undef GGZDDATADIR
#undef GAMEDIR
#undef DATADIR
#undef TMPDIR
#undef NDEBUG
#undef PACKAGE_SOURCE_DIR
#undef PF_LOCAL
#undef _BSD_SOURCE
#undef _GNU_SOURCE
#undef _XOPEN_SOURCE
#undef DB2_IN_DIR
#undef DB3_IN_DIR
#undef DB4_IN_DIR


/* Leave that blank line there!!  Autoheader needs it.
   If you're adding to this file, keep in mind:
   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  */

@BOTTOM@
/* keep these at the end of the generated config.h */

#ifndef HAVE_STRSEP
char* strsep(char **stringp, const char *delim);
#endif

