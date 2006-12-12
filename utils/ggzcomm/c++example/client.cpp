#include "connectx_client.h"

int main()
{
		connectx *cx = new connectx();
		cx->ggzcomm_set_fd(-1);
		cx->ggzcomm_network_main();

		return 0;
}

