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

///// GGZ Core ///////////////////////////////////////////////////

#include "GGZCore.h"

GGZCore::GGZCore()
{
	m_initialized = 0;
}

GGZCore::~GGZCore()
{
	if(m_initialized) ggzcore_destroy();
}

int GGZCore::init(int options, const char* debugfile, int debuglevel)
{
	GGZOptions opt;
	int ret;

	opt.flags = (GGZOptionFlags)options;
	opt.debug_file = (char*)debugfile;
	opt.debug_levels = (GGZDebugLevel)debuglevel;

	ret = ggzcore_init(opt);
	if(ret == 0) m_initialized = 1;

	return ret;
}

