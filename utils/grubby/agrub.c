/* agrub.c
 *
 * A GRUB ia A (G)GZ (R)eally (U)seful (B)ot
 *
 * Original: Rich Gade
 * ReWrite:  Justin Zaun
 *
 * May 23rd, 2000
 */

#include "agrub_popt.h"
#include "datatypes.h"
#include "init.h"
#include "loop.h"
#include "net.h"

int main (const int argc, const char *argv[])
{
	grubby_init();
	ggz_init();
	popt_get(argc, argv);
	net_connect();

	main_loop();

	return 1;
}
