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

///// GGZ Module /////////////////////////////////////////////////

#include "GGZCoreModule.h"
#include "GGZCoreCommon.h"

GGZCoreModule::GGZCoreModule()
{
	m_game = NULL;
	m_protocol = NULL;
	m_module = NULL;
}

GGZCoreModule::~GGZCoreModule()
{
}

unsigned int GGZCoreModule::countAll()
{
	return ggzcore_module_get_num();
}

void GGZCoreModule::init(const char* game, const char* protocol, const char *engine)
{
	m_game = (char*)game;
	m_protocol = (char*)protocol;
	m_engine = (char*)engine;
}

void GGZCoreModule::setActive(const unsigned int number)
{
	m_module = ggzcore_module_get_nth_by_type(m_game, m_engine, m_protocol, number);
}

unsigned int GGZCoreModule::count()
{
	if((!m_game) || (!m_protocol)) return 0;
	return ggzcore_module_get_num_by_type(m_game, m_engine, m_protocol);
}

int GGZCoreModule::add(const char* game, const char* version, const char* protocol, const char *engine, const char* author, const char* frontend, const char* url, const char* exe_path, const char* icon_path, const char* help_path)
{
	return ggzcore_module_add(game, version, protocol, engine, author, frontend, url, exe_path, icon_path, help_path);
}

int GGZCoreModule::launch()
{
	return ggzcore_module_launch(m_module);
}

char* GGZCoreModule::game()
{
	// inconsistency in ggzcore
	//return ggzcore_module_get_game(m_module);
	return NULL;
}

char* GGZCoreModule::version()
{
	return ggzcore_module_get_version(m_module);
}

char* GGZCoreModule::protocolVersion()
{
	return ggzcore_module_get_prot_version(m_module);
}

char *GGZCoreModule::protocolEngine()
{
	return ggzcore_module_get_prot_engine(m_module);
}

char* GGZCoreModule::author()
{
	return ggzcore_module_get_author(m_module);
}

char* GGZCoreModule::frontend()
{
	return ggzcore_module_get_frontend(m_module);
}

char* GGZCoreModule::url()
{
	return ggzcore_module_get_url(m_module);
}

char** GGZCoreModule::argv()
{
	return ggzcore_module_get_argv(m_module);
}

char* GGZCoreModule::pathIcon()
{
	return ggzcore_module_get_icon_path(m_module);
}

char* GGZCoreModule::pathHelp()
{
	return ggzcore_module_get_help_path(m_module);
}

GGZModule *GGZCoreModule::module()
{
	return m_module;
}
