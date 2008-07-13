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

#include "parser.h"

#include "series.h"
#include "eventgroup.h"
#include "event.h"
#include "participant.h"

#include <qfile.h>
#include <qstring.h>
#include <qdom.h>

Parser::Parser(const char *file)
{
	int ret = parse(file);

	m_series = NULL;

	if(ret == normal) buildTree();
}

void Parser::buildTree()
{
	for(unsigned int j = 0; j < m_eventgrouplist.count(); j++)
	{
		Eventgroup *eventgroup = m_eventgrouplist.at(j);
		for(unsigned int i = 0; i < m_eventlist.count(); i++)
		{
			Event *event = m_eventlist.at(i);
			if(event->eventgroup() == eventgroup->id())
			{
				event->setParent(eventgroup);
				eventgroup->addEvent(event);
			}
		}
	}

	if(m_serieslist.count() > 0)
	{
		Series *series = m_serieslist.at(0);
		for(unsigned int i = 0; i < m_eventgrouplist.count(); i++)
		{
			Eventgroup *eventgroup = m_eventgrouplist.at(i);
			series->addEventgroup(eventgroup);
		}

		m_series = series;
	}
}

Parser::~Parser()
{
}

int Parser::parse(const char *file)
{
	int ret;
	QDomDocument dom;
	QDomNode node;
	QDomElement el;

	QFile f(file);
	if(!f.open(IO_ReadOnly)) return invalid;

	ret = dom.setContent(&f);
	if(!ret) return invalid;

	node = dom.documentElement();
	el = node.toElement();
	if(el.tagName() != "ggzcalendar") return invalid;

	ret = normal;

	node = node.firstChild();
	while(!node.isNull())
	{
		el = node.toElement();
		if(el.tagName() == "event")
		{
			ret = parseEvent(&node);
		}
		else if(el.tagName() == "eventgroup")
		{
			ret = parseEventgroup(&node);
		}
		else if(el.tagName() == "series")
		{
			ret = parseSeries(&node);
		}
		else ret = unknown;
		node = node.nextSibling();
	}

	return ret;
}

Series *Parser::series()
{
	return m_series;
}

int Parser::parseSeries(QDomNode *pnode)
{
	QString id;
	QString title, organizer, result, registration, homepage;
	QDomNode node;
	QDomElement el;
	int ret;

	ret = normal;
	el = pnode->toElement();
	id = el.tagName();
	node = pnode->firstChild();
	while(!node.isNull())
	{
		el = node.toElement();
		if(el.tagName() == "title")
		{
			title = el.text();
		}
		else if(el.tagName() == "organizer")
		{
			organizer = el.text();
		}
		else if(el.tagName() == "result")
		{
			result = el.text();
		}
		else if(el.tagName() == "registration")
		{
			registration = el.text();
		}
		else if(el.tagName() == "homepage")
		{
			homepage = el.text();
		}
		else ret = unknown;
		node = node.nextSibling();
	}

	Series *series = new Series(organizer, registration, result, title, homepage);
	m_serieslist.append(series);

	return ret;
}

int Parser::parseEventgroup(QDomNode *pnode)
{
	QString id;
	QString title, status, image, policy, game;
	QDomNode node;
	QDomElement el;
	int ret;

	ret = normal;
	el = pnode->toElement();
	id = el.tagName();
	node = pnode->firstChild();
	while(!node.isNull())
	{
		el = node.toElement();
		if(el.tagName() == "image")
		{
			image = el.text();
		}
		else if(el.tagName() == "status")
		{
			status = el.text();
		}
		else if(el.tagName() == "title")
		{
			title = el.text();
		}
		else if(el.tagName() == "policy")
		{
			policy = el.text();
		}
		else if(el.tagName() == "game")
		{
			game = el.text();
		}
		else ret = unknown;
		node = node.nextSibling();
	}

	Eventgroup *eventgroup = new Eventgroup(title, status, image, policy, game);
	eventgroup->setId(pnode->toElement().attribute("id"));
	m_eventgrouplist.append(eventgroup);

	return ret;
}

int Parser::parseEvent(QDomNode *pnode)
{
	QString id;
	QString date, location, title, description, status, image;
	QDomNode node;
	QDomElement el;
	int ret;

	ret = normal;
	el = pnode->toElement();
	id = el.tagName();
	node = pnode->firstChild();
	while(!node.isNull())
	{
		el = node.toElement();
		if(el.tagName() == "date")
		{
			date = el.text();
		}
		else if(el.tagName() == "location")
		{
			location = el.text();
		}
		else if(el.tagName() == "title")
		{
			title = el.text();
		}
		else if(el.tagName() == "description")
		{
			description = el.text();
		}
		else if(el.tagName() == "status")
		{
			status = el.text();
		}
		else if(el.tagName() == "image")
		{
			image = el.text();
		}
		else if(el.tagName() == "participants")
		{
			ret = parseParticipants(&node);
		}
		else if(el.tagName() == "scores")
		{
		}
		else ret = unknown;
		node = node.nextSibling();
	}

	Event *event = new Event(date.toInt(), location, title, description, status, image);
	event->setEventgroup(pnode->toElement().attribute("eventgroup"));
	m_eventlist.append(event);

	for(unsigned int i = 0; i < m_participantlist.count(); i++)
	{
		Participant *p = m_participantlist.at(i);
		event->addParticipant(p);
	}
	m_participantlist.clear();

	return ret;
}

int Parser::parseParticipants(QDomNode *pnode)
{
	QString id;
	QString participant, team;
	QDomNode node;
	QDomElement el;
	int ret;

	ret = normal;
	el = pnode->toElement();
	id = el.tagName();
	node = pnode->firstChild();
	while(!node.isNull())
	{
		el = node.toElement();
		if(el.tagName() == "participant")
		{
			participant = el.text();
			team = el.attribute("team");
			Participant *p = new Participant(participant, team);
			m_participantlist.append(p);
		}
		else ret = unknown;
		node = node.nextSibling();
	}

	return ret;
}

