#ifndef _AGRUB_MESSAGES_
#define _AGRUB_MESSAGES_


/*
 *  Random meessages said when someone enters a room
 */

char *great_strings[] = {
	"Hey, Howsit?",
	"Glad to see ya again!",
	"Hello there.",
	"Welcome to the room!",
	"Howsit goin?",
	"Hello, whats up?",
	"Hey, ready for a good game or two?"
};


/*
 *  This message is given out when a users
 *  request '/help'. %s is Gurbbies username.
 */

char *help_strings[] = {
	"I am aware of the following commands:",
	"    %s: About",
	"    %s: Help",
	"    %s: Go away",
	"    %s: My name is <name>",
	"    %s: My email is <email>",
	"    %s: My url is <URL>",
	"    %s: My info is <info about yourself>",
	"    %s: My birthday is <MM-DD-YYYY>",
	"    %s: Have you seen <name>",
	"    %s: Tell <name> <A message>",
	"    %s: Whois <name>"
} ;


/*
 *  When someone asks for information about Grubby
 *  threw '/about' the following is returned.
 *  %s is Grubbies username
 */

char *about_strings[] = { 
	"Hi, I'm %s, and I'm A GRUB.",
	" ",
	"A GRUB is A Ggz Really Useful Bot.  I can",
	"remember all sorts of things, and as time",
	"goes on I will be able to learn even more!",
	" ",
	"I was coded originaly by Rich Gade, but",
	"was re-written by Justin Zaun. If you have",
	"problems with me, be sure to inform him!"
} ;

