/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.4                //
//    Copyright (C) 2000, 2001 Josef Spillner - dr_maux@users.sourceforge.net      //
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
#define KGGZVERSION "0.0.4"
#endif
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

