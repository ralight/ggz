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

///// GGZ Configuration I/O //////////////////////////////////////

#include "GGZCoreConfio.h"

GGZCoreConfio::GGZCoreConfio(const char* path, int options)
{
	m_confio = ggz_conf_parse(path, (GGZConfType)options);
}

GGZCoreConfio::~GGZCoreConfio()
{
}

int GGZCoreConfio::write(const char* section, const char* key, const char* value)
{
	return ggz_conf_write_string(m_confio, section, key, value);
}

int GGZCoreConfio::write(const char* section, const char* key, int value)
{
	return ggz_conf_write_int(m_confio, section, key, value);
}

int GGZCoreConfio::write(const char* section, const char* key, int argc, char** argv)
{
	return ggz_conf_write_list(m_confio, section, key, argc, argv);
}

char* GGZCoreConfio::read(const char* section, const char* key, const char* def)
{
	return ggz_conf_read_string(m_confio, section, key, def);
}

int GGZCoreConfio::read(const char* section, const char* key, int def)
{
	return ggz_conf_read_int(m_confio, section, key, def);
}

int GGZCoreConfio::read(const char* section, const char* key, int* argcp, char*** argvp)
{
	return ggz_conf_read_list(m_confio, section, key, argcp, argvp);
}

int GGZCoreConfio::removeKey(const char* section, const char* key)
{
	return ggz_conf_remove_key(m_confio, section, key);
}

int GGZCoreConfio::removeSection(const char* section)
{
	return ggz_conf_remove_section(m_confio, section);
}

int GGZCoreConfio::commit()
{
	return ggz_conf_commit(m_confio);
}

void GGZCoreConfio::free(void *ptr)
{
	//ggz_free(ptr);
	_ggz_free(ptr, " * ", __FILE__,  __LINE__);
}

void GGZCoreConfio::cleanup()
{
	ggz_conf_cleanup();
}

