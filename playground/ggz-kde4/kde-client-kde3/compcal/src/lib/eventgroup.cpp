//
//    Competition Calendar
//
//    Copyright (C) 2002, 2003 Josef Spillner <josef@ggzgamingzone.org>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "eventgroup.h"

#include "event.h"

Eventgroup::Eventgroup(QString title, QString status, QString image, QString policy, QString game)
{
	m_status = status;
	m_image = image;
	m_title = title;
	m_policy = policy;
	m_game = game;
}

Eventgroup::~Eventgroup()
{
}

void Eventgroup::addEvent(Event *event)
{
	m_events.append(event);
}

QString Eventgroup::status()
{
	return m_status;
}

QString Eventgroup::title()
{
	return m_title;
}

QString Eventgroup::image()
{
	return m_image;
}

QString Eventgroup::policy()
{
	return m_policy;
}

QString Eventgroup::game()
{
	return m_game;
}

QPtrList<Event> Eventgroup::events()
{
	return m_events;
}

void Eventgroup::setId(QString id)
{
	m_id = id;
}

QString Eventgroup::id()
{
	return m_id;
}

