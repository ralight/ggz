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

///// GGZ Configuration I/O //////////////////////////////////////

#include "GGZCoreConfio.h"

GGZCoreConfio::GGZCoreConfio(const char* path, int options)
{
	m_confio = ggzcore_confio_parse(path, (const unsigned char)options);
}

GGZCoreConfio::~GGZCoreConfio()
{
}

int GGZCoreConfio::write(const char* section, const char* key, const char* value)
{
	return ggzcore_confio_write_string(m_confio, section, key, value);
}

int GGZCoreConfio::write(const char* section, const char* key, int value)
{
	return ggzcore_confio_write_int(m_confio, section, key, value);
}

int GGZCoreConfio::write(const char* section, const char* key, int argc, char** argv)
{
	return ggzcore_confio_write_list(m_confio, section, key, argc, argv);
}

char* GGZCoreConfio::read(const char* section, const char* key, const char* def)
{
	return ggzcore_confio_read_string(m_confio, section, key, def);
}

int GGZCoreConfio::read(const char* section, const char* key, int def)
{
	return ggzcore_confio_read_int(m_confio, section, key, def);
}

int GGZCoreConfio::read(const char* section, const char* key, int* argcp, char*** argvp)
{
	return ggzcore_confio_read_list(m_confio, section, key, argcp, argvp);
}

int GGZCoreConfio::removeKey(const char* section, const char* key)
{
	return ggzcore_confio_remove_key(m_confio, section, key);
}

int GGZCoreConfio::removeSection(const char* section)
{
	return ggzcore_confio_remove_section(m_confio, section);
}

int GGZCoreConfio::commit()
{
	return ggzcore_confio_commit(m_confio);
}
