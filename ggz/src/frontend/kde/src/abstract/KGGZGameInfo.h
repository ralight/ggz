/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.5pre             //
//    Copyright (C) 2000 - 2002 Josef Spillner - dr_maux@users.sourceforge.net     //
//    The MindX Open Source Project - http://mindx.sourceforge.net                 //
//    Published under GNU GPL conditions - view COPYING for details                //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    This program is free software; you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by         //
//    the Free Software Foundation; either version 2 of the License, or            //
//    (at your option) any later version.                                          //
//                                                                                 //
//    This program is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of               //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                //
//    GNU General Public License for more details.                                 //
//                                                                                 //
//    You should have received a copy of the GNU General Public License            //
//    along with this program; if not, write to the Free Software                  //
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// KGGZGameInfo: Handle game start, table launching/joining and the related state. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_GAMEINFO_H
#define KGGZ_GAMEINFO_H

// Actions and State structure
class KGGZGameInfo
{
	public:
		// Constructor: Create a state object
		KGGZGameInfo();
		// Destructor: Delete this object
		~KGGZGameInfo();

		// Types of this object
		enum Types
		{
			typenone,
			typelaunch,
			typejoin,
			typespectator
		};

		// Set the type
		void setType(int type);
		// Assign frontend
		void setFrontend(int frontend);
		// Assign table number
		void setTable(int table);

		// Get the type
		int type();
		// Get the frontend
		int frontend();
		// Get the table number
		int table();
		
	private:
		// Frontend number
		int m_frontend;
		// Table number
		int m_table;
		// Type
		int m_type;
};

#endif

