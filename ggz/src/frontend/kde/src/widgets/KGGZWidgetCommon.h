#ifndef KGGZ_WIDGET_COMMON_H
#define KGGZ_WIDGET_COMMON_H

////////////////////////////////////////////////////////////////
// public part                                                //
////////////////////////////////////////////////////////////////

// hm, duplicate work ...
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

#endif
