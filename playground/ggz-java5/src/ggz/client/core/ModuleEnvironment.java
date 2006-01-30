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
 * @brief The environment a game frontend expects.
 * 
 * Core clients should offer those game modules which fit their own environment.
 */
public enum ModuleEnvironment {
    /** < No GUI, no interaction with user */
    GGZ_ENVIRONMENT_PASSIVE,
    /** < Text console */
    GGZ_ENVIRONMENT_CONSOLE,
    /** < VESA or framebuffer */
    GGZ_ENVIRONMENT_FRAMEBUFFER,
    /** < X11 windowed mode (default) */
    GGZ_ENVIRONMENT_XWINDOW,
    /** < X11 fullscreen mode */
    GGZ_ENVIRONMENT_XFULLSCREEN;
}
