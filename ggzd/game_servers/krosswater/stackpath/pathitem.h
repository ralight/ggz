//////////////////////////////////////////
//                                      //
// Stackpath 0.1                        //
//                                      //
// Copyright (C) 2001 Josef Spillner    //
// dr_maux@users.sourceforge.net        //
// The MindX Open Source Project        //
// http://mindx.sourceforge.net         //
//                                      //
// Published under GNU GPL conditions.  //
//                                      //
//////////////////////////////////////////

// System includes
#include <list>

using namespace std;

/**
* This class does all the calculation on its own by inheriting itself.
* It uses a list which gets passed from and to its ancestors.
*/
class Pathitem
{
	public:
		/** Constuctor: Takes a map and its size. */
		Pathitem(int width, int height, int** field);
		/** Destructor. */
		~Pathitem();
		/** The main function. Needs only called once from the programmer. */
		void process(int fromx, int fromy, int tox, int toy, int mode);
		/** Assigned horizontal position. */
		int x();
		/** Assigned vertical position. */
		int y();
		/** Status (one of the States listed below). */
		int status();
		/** The parent object, or NULL. */
		Pathitem* parent();
		/** Clean up a processed path */
		void clear();
		/** Change the status */
		void setStatus(int status);

		/** Possible states. Only 'final' is currently interesting for the programmer. */
		enum States
		{
			queued,
			processed,
			final,
			invalid
		};

		/** Process modes. Normal does only allow horizontal and vertical moves. Nostop calculates everything. */
		enum Modes
		{
			normal = 1,
			extended = 2,
			nostop = 4
		};

	protected:
		/** Returns the list of all path items. */
		list<Pathitem*> itemlist();

		/** Saved size of the map. */
		int m_width, m_height;
		/** Saved map. */
		int** m_field;

	private:
		/** Sets the from coordinates. */
		void setOrigin(int x, int y);
		/** Sets the to coordinates. */
		void setTarget(int x, int y);
		/** Handles over the current item list. */
		void setItemList(list<Pathitem*> list);
		/** Set the parent object (always 'this'). */
		void setParent(Pathitem* parent);
		/** The item's current status */
		int m_status;

		/** The from coordinates */
		int m_x, m_y;
		/** The to coordinates */
		int m_tox, m_toy;
		/** The parent object */
		Pathitem* m_parent;
		/** The local list of path items */
		list<Pathitem*> m_itemlist;
};
