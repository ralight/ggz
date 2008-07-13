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

///// GGZ Configuration //////////////////////////////////////////

#include "GGZCoreConf.h"

GGZCoreConf::GGZCoreConf()
{
}

GGZCoreConf::~GGZCoreConf()
{
}

int GGZCoreConf::init(const char* global, const char* local)
{
	return ggzcore_conf_initialize(global, local);
}

int GGZCoreConf::write(const char* section, const char* key, const char* value)
{
	return ggzcore_conf_write_string(section, key, value);
}

int GGZCoreConf::write(const char* section, const char* key, int value)
{
	return ggzcore_conf_write_int(section, key, value);
}

int GGZCoreConf::write(const char* section, const char* key, int argc, char** argv)
{
	return ggzcore_conf_write_list(section, key, argc, argv);
}

char* GGZCoreConf::read(const char* section, const char* key, const char* def)
{
	return ggzcore_conf_read_string(section, key, def);
}

int GGZCoreConf::read(const char* section, const char* key, int def)
{
	return ggzcore_conf_read_int(section, key, def);
}

int GGZCoreConf::read(const char* section, const char* key, int* argcp, char*** argvp)
{
	return ggzcore_conf_read_list(section, key, argcp, argvp);
}

int GGZCoreConf::removeKey(const char* section, const char* key)
{
	return ggzcore_conf_remove_key(section, key);
}

int GGZCoreConf::removeSection(const char* section)
{
	return ggzcore_conf_remove_section(section);
}

int GGZCoreConf::commit()
{
	return ggzcore_conf_commit();
}
