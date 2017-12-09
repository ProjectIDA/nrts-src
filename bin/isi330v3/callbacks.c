/*======================================================================
 *
 *  Callbacks provided to the Quanterra lib330 library
 *
 *=+===================================================================*/
#include "isi330.h"

void isi330_miniseed_callback(pointer p)
{
    printf("miniseed calback with pointer %p\n", p);
}

void isi330_state_callback(pointer p)
{
	tstate_call *state;
	state = (tstate_call *)p;
    string63 statestr;
    topstat opstat;
    enum tliberr liberr;

	printf("STATE: state_type=%u info=%u : %s\n", state->state_type, state->info,
            lib_get_statestr(lib_get_state(state->context, &liberr, &opstat), &statestr));

    /* PrintLib330Topstat(&opstat); */

	if (state->state_type == ST_STATE) {
		/* new operational state */
        printf("\tST_STATE: NEW STATE: %s\n", lib_get_statestr(lib_get_state(state->context, &liberr, &opstat), &statestr));
	} else if (state->state_type == ST_STATUS) {
		/* new status available */
		printf("\tST_STATUS: new status available\n");
	} else if (state->state_type == ST_CFG) {
		/* new configuration available */
		printf("\tST_CFG: new configuration available\n");
	} else if (state->state_type == ST_STALL) {
		/* change in stalled comlink state */
		printf("\tST_STALL: change in stalled comlink state \n");
	} else if (state->state_type == ST_PING) {
		/* subtype has ping type */
		printf("\tST_PING subtype has ping type\n");
	} else if (state->state_type == ST_TICK) {
		/* info has seconds, subtype has usecs */
		printf("\tST_TICK: info has seconds, subtype has usecs\n");
	} else if (state->state_type == ST_OPSTAT) {
		/* new operational status minute */
		printf("\tST_OPSTAT: new operational status minute\n");
	} else if (state->state_type == ST_TUNNEL) {
	 	/* tunnel response available */
	 	printf("\tST_TUNNEL: tunnel response available\n");
	}

}

void isi330_msg_callback(pointer p)
{
    tmsg_call *msg;
    msg = (tmsg_call *)p;
    string95 codestr;

    printf("MSG: # %u: context=%p  code=%d  timestamp=%ud  datatime=%u  :%s%s\n", msg->msgcount, msg->context, msg->code, msg->timestamp, msg->datatime, lib_get_msg(msg->code, &codestr),  msg->suffix);
}
