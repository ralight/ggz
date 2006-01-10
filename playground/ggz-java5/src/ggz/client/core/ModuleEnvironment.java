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
