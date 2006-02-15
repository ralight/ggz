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
 * The environment a game frontend expects.
 * 
 * Core clients should offer those game modules which fit their own environment.
 */
public class ModuleEnvironment {
	/** No GUI, no interaction with user */
	public static final ModuleEnvironment GGZ_ENVIRONMENT_PASSIVE = new ModuleEnvironment();

	/** Text console */
	public static final ModuleEnvironment GGZ_ENVIRONMENT_CONSOLE = new ModuleEnvironment();

	/** VESA or framebuffer */
	public static final ModuleEnvironment GGZ_ENVIRONMENT_FRAMEBUFFER = new ModuleEnvironment();

	/** X11 windowed mode (default) */
	public static final ModuleEnvironment GGZ_ENVIRONMENT_XWINDOW = new ModuleEnvironment();

	/** X11 fullscreen mode */
	public static final ModuleEnvironment GGZ_ENVIRONMENT_XFULLSCREEN = new ModuleEnvironment();
	
	private ModuleEnvironment() {}
}
