#!/usr/bin/env python
#
# MetAggregator - Aggregation of metaservers
# Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

# FIXME: helper method wrapper class?
def find(element, childname):
	# FIXME: implement more efficient search (no recursive search needed!)
	return element.getElementsByTagName(childname)

def findone(element, childname):
	children = find(element, childname)
	if len(children) == 0:
		return None
	child = children[0]
	return child

def findone_value(element, childname):
	child = findone(element, childname)
	if not child:
		return None
	textparts = child.childNodes
	for textpart in textparts:
		if textpart.nodeType == textpart.TEXT_NODE:
				text = textpart.nodeValue
				return text

	return None

