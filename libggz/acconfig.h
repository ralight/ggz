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
#undef HAVE_CMSG_ALIGN
#undef HAVE_CMSG_LEN
#undef HAVE_CMSG_SPACE
#undef HAVE_MSGHDR_MSG_CONTROL
#undef HAVE_SUN_LEN
#undef NDEBUG
#undef PACKAGE_SOURCE_DIR
#undef PF_LOCAL
#undef USE_GCRYPT
#undef GGZ_TLS_GNUTLS
#undef GGZ_TLS_NONE
#undef GGZ_TLS_OPENSSL
    

/* Leave that blank line there!!  Autoheader needs it.
   If you're adding to this file, keep in mind:
   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  */

@BOTTOM@
