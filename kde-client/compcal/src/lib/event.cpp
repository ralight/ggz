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

#include "event.h"

#include "participant.h"

#include <qdatetime.h>

Event::Event(int date, QString location, QString title, QString description, QString status, QString image)
{
	m_date = date;
	m_location = location;
	m_title = title;
	m_description = description;
	m_status = status;
	m_image = image;
}

Event::~Event()
{
}

void Event::setParent(Eventgroup *parent)
{
	m_parent = parent;
}

Eventgroup *Event::parent()
{
	return m_parent;
}

void Event::addParticipant(Participant *participant)
{
	m_participants.append(participant);
}

QString Event::date()
{
	QDateTime d;

	d.setTime_t(m_date);
	return d.toString();
}

QString Event::title()
{
	return m_title;
}

QString Event::description()
{
	return m_description;
}

QString Event::location()
{
	return m_location;
}

QPtrList<Participant> Event::participants()
{
	return m_participants;
}

QString Event::status()
{
	return m_status;
}

QString Event::image()
{
	return m_image;
}

void Event::setEventgroup(QString eventgroup)
{
	m_eventgroup = eventgroup;
}

QString Event::eventgroup()
{
	return m_eventgroup;
}

