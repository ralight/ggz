#ifndef KGGZ_COMMON_H
#define KGGZ_COMMON_H

////////////////////////////////////////////////////////////////
// public part                                                //
////////////////////////////////////////////////////////////////

// compile-time defaults can be set here
// this file is generated automatically when building with autoconf
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define KGGZ_DIRECTORY "/usr/local/share/kggz"
#endif

// setkggzdebug:
// set 0 or 1, whatever you want
#ifndef SETKGGZDEBUG
#define SETKGGZDEBUG 1
#endif

// version
// should always stick to GGZ version numbers
#ifndef KGGZVERSION
#define KGGZVERSION "0.0.4pre6"
#endif

////////////////////////////////////////////////////////////////
// private part                                               //
////////////////////////////////////////////////////////////////

// KGGZDEBUG:
// seems like every app has its own debug function... cool...
#ifdef SETKGGZDEBUG
#include <stdio.h>
#define KGGZDEBUGF(fmt...) printf("KGGZ >> "fmt##)
#define KGGZDEBUG(fmt...) printf(">>> "fmt##)
#else
#define KGGZDEBUGF(fmt...)
#define KGGZDEBUG(fmt...)
#endif

// GGZ includes
#include <ggzcore.h>

class KGGZCommon
{
	public:
		static char* state(GGZStateID stateid);
		static int launchProcess(char* process, char* processpath);
		static int killProcess(char* process);
		static int findProcess(char* cmdline);
		static char* append(char* string1, char* string2);
		static void clear();
	private:
};

#endif
