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

int GGZCore::init(int options)
{
	GGZOptions opt;
	int ret;

	opt.flags = (GGZOptionFlags)options;

	ret = ggzcore_init(opt);
	if(ret == 0) m_initialized = 1;

	return ret;
}

void GGZCore::reload()
{
	ggzcore_reload();
}

