/** Omnicracklib - a password strength check library
 *  Copyright (C) 2007 Josef Spillner
 *
 *  Omnicracklib is intended to be a password checker similar to cracklib.
 *  It takes a password and evaluates if the password could easily be
 *  guessed by intruders. Server authors are encouraged to include
 *  omnicrack checking for whenever users are allowed to modify their
 *  passwords.
 */

#ifndef OMNICRACK_HASHER_H
#define OMNICRACK_HASHER_H

/*
 * Converts a file containing one word per line into a hash table
 * on disk.
 * Returns 1 on success, -1 on failure.
 */
int hashfile(const char *wordlistfilename, const char *hashfilename);

/*
 * Checks if the word appears in the hash table file.
 * Returns 1 on cache hit, 0 on miss, -1 on failure.
 */
int findword(const char *hashfilename, const char *word);

#endif
