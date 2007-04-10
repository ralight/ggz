/** Omnicracklib - a password strength check library
 *  Copyright (C) 2007 Josef Spillner
 *
 *  Omnicracklib is intended to be a password checker similar to cracklib.
 *  It takes a password and evaluates if the password could easily be
 *  guessed by intruders. Server authors are encouraged to include
 *  omnicrack checking for whenever users are allowed to modify their
 *  passwords.
 */

#ifndef OMNICRACK_H
#define OMNICRACK_H

/* Check if the password is long enough to be considered secure */
#define OMNICRACK_STRATEGY_LENGTH    0x01
/* Check if the password stretches across several unicode blocks */
#define OMNICRACK_STRATEGY_BLOCKS    0x02
/* Check if letters, numbers, spaces etc. are all used */
#define OMNICRACK_STRATEGY_VARIETY   0x04
/* Check against usernames in the /etc/passwd file */
#define OMNICRACK_STRATEGY_PASSWD    0x10
/* Check against words from a dictionary - dictpath must not be NULL then */
#define OMNICRACK_STRATEGY_DICT      0x20
/* Check parts of a word against a dictionary - also needs dictpath */
#define OMNICRACK_STRATEGY_DICTSLICE 0x40
/* All of the above checks combined - this should be used by default */
#define OMNICRACK_STRATEGY_ALL       0xFF

/* The password is a good one */
#define OMNICRACK_RESULT_OK        0
/* The password is too short */
#define OMNICRACK_RESULT_BADLENGTH 1
/* The password was found in the dictionary */
#define OMNICRACK_RESULT_BADDICT   2
/* The password failed at other tests */
#define OMNICRACK_RESULT_BADSCORE  3
/* Internal error, e.g. Unicode conversion */
#define OMNICRACK_RESULT_BADINPUT  4

/** The central password strength check function
 *
 *  Takes a password, a strategy or combination of strategies, and depending
 *  on the strategies a path to a hashfile containing dictionary words.
 *  Returns a result which indicates if the password is considered strong.
 */
int omnicrack_checkstrength(const char *password, int strategies, const char *dictpath);

#endif

