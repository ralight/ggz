// This is the config-file for StarWars.

// Prefixes (don't forget to put the files there)
#ifdef RELEASE
#define PREFIX		"/usr/share/games/starwars"
#else
#define PREFIX          "data"
#endif

// Controls
#define KEY_QUIT	SDLK_q
#define KEY_PAUSE	SDLK_p
#define KEY_UP		SDLK_UP
#define KEY_DOWN	SDLK_DOWN
#define KEY_LEFT	SDLK_LEFT
#define KEY_RIGHT	SDLK_RIGHT
#define KEY_FIRE	SDLK_SPACE
#define KEY_ROCKET	SDLK_RETURN
#define USE_JOYSTICK

// Additional configuration
#define MAX_EN		15	// Number of enemies (max. 15)
#define FOV		80	// The field of view of your enemies
