/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.5pre             //
//    Copyright (C) 2000 - 2002 Josef Spillner - dr_maux@users.sourceforge.net     //
//    The MindX Open Source Project - http://mindx.sourceforge.net                 //
//    Published under GNU GPL conditions - view COPYING for details                //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    This program is free software; you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by         //
//    the Free Software Foundation; either version 2 of the License, or            //
//    (at your option) any later version.                                          //
//                                                                                 //
//    This program is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of               //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                //
//    GNU General Public License for more details.                                 //
//                                                                                 //
//    You should have received a copy of the GNU General Public License            //
//    along with this program; if not, write to the Free Software                  //
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// KGGZCommon: Contains macros, definitions, constants and versioning information. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

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
#warning No configuration file included! Using defaults!
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
#ifdef VERSION
#define KGGZVERSION VERSION
#else
#define KGGZVERSION "0.0.8pre"
#endif
#endif

// wallet support
#include <kdeversion.h>
#if (KDE_VERSION_MAJOR > 3) || ((KDE_VERSION_MAJOR == 3) && (KDE_VERSION_MINOR >= 2)) || (KDE_VERSION_RELEASE >= 90)
#define KGGZ_WALLET
#endif

////////////////////////////////////////////////////////////////
// private part                                               //
////////////////////////////////////////////////////////////////

// KGGZDEBUG:
// seems like every app has its own debug function... cool...
#if SETKGGZDEBUG == 1
  #include <stdio.h>
  #ifdef __STRICT_ANSI__
    #define KGGZDEBUG KGGZCommon::kggzdebugdummy
    #define KGGZDEBUGF KGGZDEBUG
    //#ifndef __USE_ISO9CX
    //  #define __USE_ISO9CX
    //#endif
    #ifndef atoll
      #define atoll(x) atol(x)
    #endif
    #ifndef strdup
      #define strdup(x) strcpy(((char*)malloc(strlen(x) + 1)), x)
    #endif
  #else
    #define KGGZDEBUGF(fmt, args...) printf("KGGZ >> " fmt, ##args)
    #define KGGZDEBUG(fmt, args...) printf(">>> " fmt, ##args)
  #endif
#else
  #define KGGZDEBUGF(fmt...)
  #define KGGZDEBUG(fmt...)
#endif

// GGZ includes
#include <ggzcore.h>

class KGGZCommon
{
	public:
		static const char* state(GGZStateID stateid);
		static int launchProcess(const char* process, char* processpath);
		static int killProcess(const char* process);
		static int findProcess(const char* cmdline);
		static int kggzdebugdummy(const char *x, ...);
};

#endif

