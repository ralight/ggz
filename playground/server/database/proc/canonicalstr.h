#ifndef CANONICALSTR_H
#define CANONICALSTR_H

/*
 * Function to return the normalized version of a string (e.g. username)
 * according to a "stringprep" profile.
 *
 * This code runs as a plugin directly in the database server. As such
 * it is subject to careful changes only!
 */
char *username_canonical(const char *username);

#endif
