/*
    This file is part of the kggzcore library.
    Copyright (c) 2009 Josef Spillner <josef@ggzgamingzone.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "module.h"

using namespace KGGZCore;

Module::Module()
{
}

void Module::setName(const QString& name)
{
	m_name = name;
}

QString Module::name() const
{
	return m_name;
}

void Module::setVersion(const QString& version)
{
	m_version = version;
}

QString Module::version() const
{
	return m_version;
}

void Module::setProtocolEngine(const QString& protocolengine)
{
	m_protocolengine = protocolengine;
}

QString Module::protocolEngine() const
{
	return m_protocolengine;
}

void Module::setProtocolVersion(const QString& protocolversion)
{
	m_protocolversion = protocolversion;
}

QString Module::protocolVersion() const
{
	return m_protocolversion;
}

void Module::setAuthor(const QString& author)
{
	m_author = author;
}

QString Module::author() const
{
	return m_author;
}

void Module::setFrontend(const QString& frontend)
{
	m_frontend = frontend;
}

QString Module::frontend() const
{
	return m_frontend;
}

void Module::setHomepage(const QString& homepage)
{
	m_homepage = homepage;
}

QString Module::homepage() const
{
	return m_homepage;
}

void Module::setHelpPath(const QString& helppath)
{
	m_helppath = helppath;
}

QString Module::helpPath() const
{
	return m_helppath;
}

void Module::setIconPath(const QString& iconpath)
{
	m_iconpath = iconpath;
}

QString Module::iconPath() const
{
	return m_iconpath;
}

void Module::setEnvironment(const Environment& environment)
{
	m_environment = environment;
}

Module::Environment Module::environment() const
{
	return m_environment;
}

