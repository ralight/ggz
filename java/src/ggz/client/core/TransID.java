/*
 * Copyright (C) 2006  Helg Bredow
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
package ggz.client.core;

import java.util.logging.Logger;

/*
 * Transition IDs for all of the transitions the GGZ state machine makes. These
 * serve as inputs for the state machine
 */
public class TransID {
	public static final TransID GGZ_TRANS_CONN_TRY = new TransID();

	public static final TransID GGZ_TRANS_CONN_OK = new TransID();

	public static final TransID GGZ_TRANS_CONN_FAIL = new TransID();

	public static final TransID GGZ_TRANS_LOGIN_TRY = new TransID();

	public static final TransID GGZ_TRANS_LOGIN_OK = new TransID();

	public static final TransID GGZ_TRANS_LOGIN_FAIL = new TransID();

	public static final TransID GGZ_TRANS_ENTER_TRY = new TransID();

	public static final TransID GGZ_TRANS_ENTER_OK = new TransID();

	public static final TransID GGZ_TRANS_ENTER_FAIL = new TransID();

	public static final TransID GGZ_TRANS_LAUNCH_TRY = new TransID();

	public static final TransID GGZ_TRANS_LAUNCH_OK = new TransID();

	public static final TransID GGZ_TRANS_LAUNCH_FAIL = new TransID();

	public static final TransID GGZ_TRANS_JOIN_TRY = new TransID();

	public static final TransID GGZ_TRANS_JOIN_OK = new TransID();

	public static final TransID GGZ_TRANS_JOIN_FAIL = new TransID();

	public static final TransID GGZ_TRANS_LEAVE_TRY = new TransID();

	public static final TransID GGZ_TRANS_LEAVE_OK = new TransID();

	public static final TransID GGZ_TRANS_LEAVE_FAIL = new TransID();

	public static final TransID GGZ_TRANS_LOGOUT_TRY = new TransID();

	public static final TransID GGZ_TRANS_LOGOUT_OK = new TransID();

	public static final TransID GGZ_TRANS_NET_ERROR = new TransID();

	public static final TransID GGZ_TRANS_PROTO_ERROR = new TransID();

	static final Logger log = Logger.getLogger(TransID.class.getName());

	/* Structure to represent state transition pairs */
	private static class _Transition {

		/* Transition ID */
		TransID id;

		/* Next state */
		StateID next;

		_Transition(TransID id, StateID next) {
			this.id = id;
			this.next = next;
		}
	}

	/* Structure for a particular client state */
	private static class _State {

		/* Unique id number */
		StateID id;

		/* Descriptive string (mainly for debugging purposes) */
		String name;

		/* Array of valid state transitions */
		_Transition[] transitions;

		_State(StateID id, String name, _Transition[] transitions) {
			this.id = id;
			this.name = name;
			this.transitions = transitions;
		}
	}

	/* Giant list of transitions for each state */
	private static _Transition[] _offline_transitions = new _Transition[] {
			new _Transition(GGZ_TRANS_CONN_TRY, StateID.GGZ_STATE_CONNECTING),
			new _Transition(null, null) };

	private static _Transition[] _connecting_transitions = new _Transition[] {
			new _Transition(GGZ_TRANS_CONN_OK, StateID.GGZ_STATE_ONLINE),
			new _Transition(GGZ_TRANS_CONN_FAIL, StateID.GGZ_STATE_OFFLINE),
			new _Transition(null, null) };

	private static _Transition[] _reconnecting_transitions = new _Transition[] {
			new _Transition(GGZ_TRANS_CONN_OK, StateID.GGZ_STATE_ONLINE),
			new _Transition(GGZ_TRANS_CONN_FAIL, StateID.GGZ_STATE_RECONNECTING),
			new _Transition(null, null) };

	private static _Transition[] _online_transitions = new _Transition[] {
			new _Transition(GGZ_TRANS_LOGIN_TRY, StateID.GGZ_STATE_LOGGING_IN),
			new _Transition(GGZ_TRANS_LOGOUT_TRY, StateID.GGZ_STATE_LOGGING_OUT),
			new _Transition(GGZ_TRANS_NET_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(GGZ_TRANS_PROTO_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(null, null) };

	private static _Transition[] _logging_in_transitions = new _Transition[] {
			new _Transition(GGZ_TRANS_LOGIN_OK, StateID.GGZ_STATE_LOGGED_IN),
			new _Transition(GGZ_TRANS_LOGIN_FAIL, StateID.GGZ_STATE_ONLINE),
			new _Transition(GGZ_TRANS_LOGOUT_TRY, StateID.GGZ_STATE_LOGGING_OUT),
			new _Transition(GGZ_TRANS_NET_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(GGZ_TRANS_PROTO_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(null, null) };

	private static _Transition[] _logged_in_transitions = new _Transition[] {
			new _Transition(GGZ_TRANS_ENTER_TRY,
					StateID.GGZ_STATE_ENTERING_ROOM),
			new _Transition(GGZ_TRANS_LOGOUT_TRY, StateID.GGZ_STATE_LOGGING_OUT),
			new _Transition(GGZ_TRANS_NET_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(GGZ_TRANS_PROTO_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(null, null) };

	private static _Transition[] _entering_room_transitions = new _Transition[] {
			new _Transition(GGZ_TRANS_ENTER_OK, StateID.GGZ_STATE_IN_ROOM),
			new _Transition(GGZ_TRANS_ENTER_FAIL, StateID.GGZ_STATE_LOGGED_IN),
			new _Transition(GGZ_TRANS_LOGOUT_TRY, StateID.GGZ_STATE_LOGGING_OUT),
			new _Transition(GGZ_TRANS_NET_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(GGZ_TRANS_PROTO_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(null, null) };

	private static _Transition[] _in_room_transitions = new _Transition[] {
			new _Transition(GGZ_TRANS_ENTER_TRY,
					StateID.GGZ_STATE_BETWEEN_ROOMS),
			new _Transition(GGZ_TRANS_LAUNCH_TRY,
					StateID.GGZ_STATE_LAUNCHING_TABLE),
			new _Transition(GGZ_TRANS_JOIN_TRY, StateID.GGZ_STATE_JOINING_TABLE),
			new _Transition(GGZ_TRANS_LOGOUT_TRY, StateID.GGZ_STATE_LOGGING_OUT),
			new _Transition(GGZ_TRANS_NET_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(GGZ_TRANS_PROTO_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(null, null) };

	private static _Transition[] _between_rooms_transitions = new _Transition[] {
			new _Transition(GGZ_TRANS_ENTER_OK, StateID.GGZ_STATE_IN_ROOM),
			new _Transition(GGZ_TRANS_ENTER_FAIL, StateID.GGZ_STATE_IN_ROOM),
			new _Transition(GGZ_TRANS_LOGOUT_TRY, StateID.GGZ_STATE_LOGGING_OUT),
			new _Transition(GGZ_TRANS_NET_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(GGZ_TRANS_PROTO_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(null, null) };

	private static _Transition[] _launching_table_transitions = new _Transition[] {
	/* For now, server automatically tries to join us */
	new _Transition(GGZ_TRANS_LAUNCH_OK, StateID.GGZ_STATE_JOINING_TABLE),
			new _Transition(GGZ_TRANS_LAUNCH_FAIL, StateID.GGZ_STATE_IN_ROOM),
			new _Transition(GGZ_TRANS_NET_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(GGZ_TRANS_PROTO_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(null, null) };

	private static _Transition[] _joining_table_transitions = new _Transition[] {
			new _Transition(GGZ_TRANS_JOIN_OK, StateID.GGZ_STATE_AT_TABLE),
			new _Transition(GGZ_TRANS_JOIN_FAIL, StateID.GGZ_STATE_IN_ROOM),
			new _Transition(GGZ_TRANS_NET_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(GGZ_TRANS_PROTO_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(null, null) };

	private static _Transition[] _at_table_transitions = new _Transition[] {
			new _Transition(GGZ_TRANS_LEAVE_TRY,
					StateID.GGZ_STATE_LEAVING_TABLE),
			new _Transition(GGZ_TRANS_LEAVE_OK, StateID.GGZ_STATE_IN_ROOM),
			new _Transition(GGZ_TRANS_NET_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(GGZ_TRANS_PROTO_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(null, null) };

	private static _Transition[] _leaving_table_transitions = new _Transition[] {
			new _Transition(GGZ_TRANS_LEAVE_OK, StateID.GGZ_STATE_IN_ROOM),
			new _Transition(GGZ_TRANS_LEAVE_FAIL, StateID.GGZ_STATE_AT_TABLE),
			new _Transition(GGZ_TRANS_NET_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(GGZ_TRANS_PROTO_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(null, null) };

	private static _Transition[] _logging_out_transitions = new _Transition[] {
			new _Transition(GGZ_TRANS_NET_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(GGZ_TRANS_PROTO_ERROR, StateID.GGZ_STATE_OFFLINE),
			new _Transition(GGZ_TRANS_LOGOUT_OK, StateID.GGZ_STATE_OFFLINE),
			new _Transition(null, null) };

	/* Array of all GGZ states */
	static final _State[] __states = new _State[] {
			new _State(StateID.GGZ_STATE_OFFLINE, "offline",
					_offline_transitions),
			new _State(StateID.GGZ_STATE_CONNECTING, "connecting",
					_connecting_transitions),
			new _State(StateID.GGZ_STATE_RECONNECTING, "reconnecting",
					_reconnecting_transitions),
			new _State(StateID.GGZ_STATE_ONLINE, "online", _online_transitions),
			new _State(StateID.GGZ_STATE_LOGGING_IN, "logging_in",
					_logging_in_transitions),
			new _State(StateID.GGZ_STATE_LOGGED_IN, "logged_in",
					_logged_in_transitions),
			new _State(StateID.GGZ_STATE_ENTERING_ROOM, "entering_room",
					_entering_room_transitions),
			new _State(StateID.GGZ_STATE_IN_ROOM, "in_room",
					_in_room_transitions),
			new _State(StateID.GGZ_STATE_BETWEEN_ROOMS, "between_rooms",
					_between_rooms_transitions),
			new _State(StateID.GGZ_STATE_LAUNCHING_TABLE, "launching_table",
					_launching_table_transitions),
			new _State(StateID.GGZ_STATE_JOINING_TABLE, "joining_table",
					_joining_table_transitions),
			new _State(StateID.GGZ_STATE_AT_TABLE, "at_table",
					_at_table_transitions),
			new _State(StateID.GGZ_STATE_LEAVING_TABLE, "leaving_table",
					_leaving_table_transitions),
			new _State(StateID.GGZ_STATE_LOGGING_OUT, "logging_out",
					_logging_out_transitions), };

	public StateID state_transition(StateID cur) {
		TransID trans = this;
		int i = 0;
		_Transition[] transitions;
		StateID next = null;

		transitions = __states[cur.ordinal()].transitions;

		/* Look through valid transitions to see if this one is OK */
		while (transitions[i].id != null) {
			if (transitions[i].id == trans) {
				next = transitions[i].next;
				break;
			}
			++i;
		}

		if (next != cur && next != null) {
			log.fine("State transition " + __states[cur.ordinal()].name
					+ " -> " + __states[next.ordinal()].name);
			return next;
		}
        log.warning("No transitions found " + __states[cur.ordinal()].name);
		return cur;
	}
}
