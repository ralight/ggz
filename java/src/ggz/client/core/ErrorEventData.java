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

import ggz.common.ClientReqError;

/**
 * @brief The data describing an error.
 * 
 * When an error occurrs, a pointer to a struct of this type will be passed as
 * the event data.
 */
public class ErrorEventData {

    /**
     * @brief A default error message.
     * @note This also provides backward-compatability.
     */
    public String message;

    /**
     * @brief The type of error that occurred.
     * @note Not all errors are possible with all events.
     */
    public ClientReqError status;
    
    ErrorEventData(ClientReqError code) {
        this.status = code;
    }
}
