/*======================================================================
 *
 *  Callbacks provided to the Quanterra lib330 library
 *
 *=+===================================================================*/
#include <libclient.h>
#include "isi330.h"

void isi330_miniseed_callback(pointer p)
{
    tminiseed_call *msp;
    char buf[256];

    msp = (tminiseed_call *)p;

    if (msp->chan_number == 0xFF) LogMsg("\nDP STATISTICS\n");

    LogMsg("LIB330 CALLBACK (MINISEED): with pointer %p\n", p);
    LogMsg("            station_name: %s", msp->station_name);
    LogMsg("                location: %s", msp->location);
    LogMsg("   chan_number (token #): %u", msp->chan_number);
    LogMsg("                 channel: %s", msp->channel);
    LogMsg("                    rate: %d", msp->rate);
//  LogMsg("             cl_session: %lu", msp->cl_session);
//  LogMsg("               cl_offset: %f", msp->cl_offset);
    LogMsg("               timestamp: %f", msp->timestamp);
    LogMsg("             filter_bits: %u", msp->filter_bits);
    LogMsg("            packet_class: %s [%u]",
           lib330PacketClassString(msp->packet_class),
           msp->packet_class);
    LogMsg("         miniseed_action: %u", msp->miniseed_action);
    LogMsg("               data_size: %u", msp->data_size);
    LogMsg("            data_address: %p", msp->data_address);

    //    do something
    switch (msp->packet_class) {
        case PKC_DATA: break;
        case PKC_EVENT: break;
        case PKC_CALIBRATE: break;
        case PKC_TIMING: break;
        case PKC_MESSAGE: break;
        case PKC_OPAQUE: break;
        default: break;
    }

}

void isi330_state_callback(pointer p)
{
	tstate_call *state;
	state = (tstate_call *)p;
    string63 statestr;
    topstat opstat;
    enum tliberr liberr;

	LogMsg("LIB330 CALLBACK (STATE): state_type=%u info=%u : %s\n", state->state_type, state->info,
            lib_get_statestr(lib_get_state(state->context, &liberr, &opstat), &statestr));

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

    LogMsg("LIB330 CALLBACK (MSG): # %u [code=%d]: %s%s\n", msg->msgcount, msg->code, lib_get_msg(msg->code, &codestr),  msg->suffix);
}
