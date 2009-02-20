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

#ifndef KGGZCORE_MODULE_H
#define KGGZCORE_MODULE_H

#include <QString>

#include "kggzcore_export.h"

namespace KGGZCore
{

class KGGZCORE_EXPORT Module
{
	public:
		Module();

		enum Environment
		{
			passive,
			console,
			framebuffer,
			xwindow,
			xfullscreen
		};

		void setName(const QString& name);
		QString name() const;

		void setVersion(const QString& version);
		QString version() const;

		void setProtocolEngine(const QString& protocolengine);
		QString protocolEngine() const;

		void setProtocolVersion(const QString& protocolversion);
		QString protocolVersion() const;

		void setAuthor(const QString& author);
		QString author() const;

		void setFrontend(const QString& frontend);
		QString frontend() const;

		void setHomepage(const QString& homepage);
		QString homepage() const;

		void setHelpPath(const QString& helppath);
		QString helpPath() const;

		void setIconPath(const QString& iconpath);
		QString iconPath() const;

		void setEnvironment(const Environment& environment);
		Environment environment() const;

	private:
		QString m_name;
		QString m_version;
		QString m_protocolengine;
		QString m_protocolversion;
		QString m_author;
		QString m_frontend;
		QString m_homepage;
		QString m_helppath;
		QString m_iconpath;
		Environment m_environment;
};

}

#endif
