/////////////////////////////////////////////////////////////////////
//                                                                 ///
// GGZCore++ - C++ API wrapper for the ggzcore library              ///
// (C) 2001 Josef Spillner                                           ///
// dr_maux@users.sourceforge.net                                      ///
// The MindX Open Source Project                                      ///
// http://mindx.sourceforge.net/                                     ///
//                                                                  ///
// This library may be distributed under the terms of the GNU GPL. ///
//                                                                ///
////////////////////////////////////////////////////////////////////

///// GGZ Common /////////////////////////////////////////////////

#ifndef GGZCOMMON_H
#define GGZCOMMON_H

#ifndef NULL
#define NULL 0
#endif

#ifndef GGZCOREDEBUG
#include <stdio.h>
#define GGZCOREDEBUG(fmt...) printf("))) "fmt##)
#endif

#endif
