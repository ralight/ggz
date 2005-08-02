
/*
 * File: display.h
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 7/26/97
 *
 * This file contains the declarations for the display functions
 *
 * Copyright (C) 1998 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */


#include <card.h>

/**
 * Initialize display and declare windows and panels and such
 */
void DisplayInit(void);


/**
 *  Input server name if not already obtained
 */
void InputServerInfo(void);


/**
 * Main control loop
 */
int MainIOLoop(void);


/**
 * Display a line of text in status window
 */
void DisplayStatusLine(const char *, ...);


/**
  * Display user input Prompt
  */
void DisplayPrompt(void);


/**
 * Display the "card table" 
 */
void DisplayTable(void);


/**
  * Display a hand of thirteen cards
  */
void DisplayHand(void);


/**
  * Display team score in side windows
  */
void DisplayScores(void);


/**
  * Display bids and trick counts in side windows
  */
void DisplayTallys(void);


/**
  * Display a card at location x,y
  */
void DisplayCard(Card, int y, int x);


/**
  * Display a card on the table 
  */
void DisplayPlayedCard(Card, int, int);


/**
  * Display session stats
  */
void DisplayRecord(void);


/**
  * Hide a card and its label
  */
void HideCard(int);


/**
  * Various cleanup functions
  */
void DisplayCleanup(void);

/**
  * Facility to display errors
  */
void DisplayError(char *);
void DisplayWarning(char *);
