/*
    This file is part of the kggzcore library.
    Copyright (c) 2008 Josef Spillner <josef@ggzgamingzone.org>

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

#ifndef KGGZCORE_ERROR_H
#define KGGZCORE_ERROR_H

#include <QString>

namespace KGGZCore
{

enum State
{
	offline,
	connecting,
	reconnecting,
	online,
	logging_in,
	logged_in,
	entering_room,
	in_room,
	between_rooms,
	launching_table,
	joining_table,
	at_table,
	leaving_table,
	logging_out
};

class Error
{
	public:
		//Error();

		enum ErrorCode
		{
			no_status,
			ok,
			user_lookup,
			bad_options,
			room_full,
			table_full,
			table_empty,
			launch_fail,
			join_fail,
			no_table,
			leave_fail,
			leave_forbidden,
			already_logged_in,
			not_logged_in,
			not_in_room,
			at_table,
			in_transit,
			no_permission,
			bad_xml,
			seat_assign_fail,
			no_channel,
			too_long,
			unknown,
			bad_username,
			user_type,
			user_found,
			user_taken,
			bad_password
		};

		static QString errormessage(ErrorCode error);
};

}

#endif
