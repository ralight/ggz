/////////////////////////////////////////////////////////////////////
//                                                                 ///
// GGZCore++ - C++ API wrapper for the ggzcore library              ///
// Copyright (C) 2001, 2002 Josef Spillner                           ///
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
  #ifdef __STRICT_ANSI__
    #define USE_DEBUGDUMMY
    #ifdef __cplusplus
      extern "C" {
    #endif
    int debugdummy(const char *x, ...);
    #ifdef __cplusplus
      }
    #endif
    #define GGZCOREDEBUG debugdummy
  #else
    #include <stdio.h>
    #define GGZCOREDEBUG(fmt, args...) printf("))) " fmt, ##args)
  #endif
#endif

#endif

