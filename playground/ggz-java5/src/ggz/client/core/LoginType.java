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

/**
 * This controls the type of login a user chooses. A different value will
 * require different information to be sent to the server.
 */
public enum LoginType {
    /** Standard login; uname and correct passwd needed. */
    GGZ_LOGIN,

    /** Guest login; only a uname is required. */
    GGZ_LOGIN_GUEST,

    /**
     * New user login; only a uname is required. Password will be assigned by
     * the server (but can be passed along).
     */
    GGZ_LOGIN_NEW
}
