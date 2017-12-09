/*======================================================================
 *
 *  Callbacks provided to the Quanterra lib330 library
 *
 *=+===================================================================*/
#include "isi330.h"

void isi330_state_callback(pointer p)
{
	tstate_call *state;
	state = (tstate_call *)p;

	printf("state->state_type raw: %ld\n", state->state_type);

	if (state->state_type == ST_STATE) {
		/* new operational state */
		printf("/* new operational state */\n");
	} else if (state->state_type == ST_STATUS) {
		/* new status available */
		printf("/* new status available */\n");
	} else if (state->state_type == ST_CFG) {
		/* new configuration available */
		printf("/* new configuration available */\n");
	} else if (state->state_type == ST_STALL) {
		/* change in stalled comlink state */
		printf("/* change in stalled comlink state */\n");
	} else if (state->state_type == ST_PING) {
		/* subtype has ping type */
		printf("/* subtype has ping type */\n");
	} else if (state->state_type == ST_TICK) {
		/* info has seconds, subtype has usecs */
		printf("/* info has seconds, subtype has usecs */\n");
	} else if (state->state_type == ST_OPSTAT) {
		/* new operational status minute */
		printf("/* new operational status minute */\n");
	} else if (state->state_type == ST_TUNNEL) {
	 	/* tunnel response available */
	 	printf("/* tunnel response available */\n");
	}

}


