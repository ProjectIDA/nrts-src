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
    printf(lib_get_statestr((enum tlibstate)state->info, &statestr));

    /* PrintLib330Topstat(&opstat); */

	/* if (state->state_type == ST_STATE) { */
	/* 	/1* new operational state *1/ */
        /* printf("\tST_STATE: NEW STATE: %s\n", lib_get_statestr(lib_get_state(state->context, &liberr, &opstat), &statestr)); */
	/* } else if (state->state_type == ST_STATUS) { */
	/* 	/1* new status available *1/ */
	/* 	printf("\tST_STATUS: new status available\n"); */
	/* } else if (state->state_type == ST_CFG) { */
	/* 	/1* new configuration available *1/ */
	/* 	printf("\tST_CFG: new configuration available\n"); */
	/* } else if (state->state_type == ST_STALL) { */
	/* 	/1* change in stalled comlink state *1/ */
	/* 	printf("\tST_STALL: change in stalled comlink state \n"); */
	/* } else if (state->state_type == ST_PING) { */
	/* 	/1* subtype has ping type *1/ */
	/* 	printf("\tST_PING subtype has ping type\n"); */
	/* } else if (state->state_type == ST_TICK) { */
	/* 	/1* info has seconds, subtype has usecs *1/ */
	/* 	printf("\tST_TICK: info has seconds, subtype has usecs\n"); */
	/* } else if (state->state_type == ST_OPSTAT) { */
	/* 	/1* new operational status minute *1/ */
	/* 	printf("\tST_OPSTAT: new operational status minute\n"); */
	/* } else if (state->state_type == ST_TUNNEL) { */
	/*  	/1* tunnel response available *1/ */
	/*  	printf("\tST_TUNNEL: tunnel response available\n"); */
	/* } */

}

void isi330_msg_callback(pointer p)
{
    tmsg_call *msg;
    msg = (tmsg_call *)p;
    string95 codestr;

    printf("MSG: # %u [code=%d]:%s%s\n", msg->msgcount, msg->code, lib_get_msg(msg->code, &codestr),  msg->suffix);
}
