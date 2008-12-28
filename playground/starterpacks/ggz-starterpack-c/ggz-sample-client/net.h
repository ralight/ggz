/* Generated by GGZComm/ruby version 0.4 */
/* Protocol 'tictactoe', version '5' */
/* Do NOT edit this file! */

#ifndef GGZCOMM_TICTACTOE_H
#define GGZCOMM_TICTACTOE_H

#define sndmove 0
#define reqsync 1
#define reqmove 4
#define rspmove 5
#define msgmove 2
#define sndsync 6
#define msggameover 3

struct variables_t
{
	int move_c;
	char status;
	int player;
	int move;
	char turn;
	char space[9];
	char winner;
};
struct variables_t variables;

void ggzcomm_network_main(int fd);

void ggzcomm_sndmove(void);
void ggzcomm_reqsync(void);

typedef void (*notifier_func_type)(int opcode);
typedef void (*error_func_type)(void);

void ggzcomm_set_notifier_callback(notifier_func_type f);
void ggzcomm_set_error_callback(error_func_type f);

#endif

