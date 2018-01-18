/*======================================================================
 *
 *  Callbacks provided to the Quanterra lib330 library
 *
 *=+===================================================================*/
#include <libclient.h>
#include "isi330.h"

static ISI330_CONFIG *lcfg = NULL;

void InitCallbacks(ISI330_CONFIG *cfg) {
    lcfg = cfg;
}

void isi330_miniseed_callback(pointer p)
{
    static char* fid = "isi330_miniseed_callback";
    tminiseed_call *msp;
    char buf[256];
    MSEED_RECORD mseed;
    char ida1012[IDA10_FIXEDRECLEN];

    msp = (tminiseed_call *)p;

    if (msp == NULL) {
        LogMsg("%s: lib330 miniseed callback called with NULL pointer.\n", fid);
        return;
    }

    if (msp->chan_number == 0xFF) LogMsg("DP STATISTICS\n");

    /* Don't pass VH? channels through if unwanted */
    if (lcfg->dropvh && (strncmp(msp->channel, "VH", 2) == 0)) return;


    /* LogMsg("LIB330 CALLBACK (MINISEED): with pointer %p; %s %s-%s (sr: %d)\n", */
    /*         p, msp->station, msp->channel, msp->location msp->rate); */
    /* LogMsg("            station_name: %s", msp->station_name); */
    /* LogMsg("                location: %s", msp->location); */
    /* LogMsg("   chan_number (token #): %u", msp->chan_number); */
    /* LogMsg("                 channel: %s", msp->channel); */
    /* LogMsg("                    rate: %d", msp->rate); */
    //  LogMsg("             cl_session: %lu", msp->cl_session);
    //  LogMsg("               cl_offset: %f", msp->cl_offset);
    /* LogMsg("               timestamp: %f", msp->timestamp); */
    /* LogMsg("             filter_bits: %u", msp->filter_bits); */
    /* LogMsg("            packet_class: %s [%u]", */
    /*        lib330PacketClassString(msp->packet_class), */
    /*        msp->packet_class); */
    /* LogMsg("         miniseed_action: %u", msp->miniseed_action); */
    /* LogMsg("               data_size: %u", msp->data_size); */
    /* LogMsg("            data_address: %p", msp->data_address); */

    //    do something
    switch (msp->packet_class) {
        case PKC_DATA:
            /* LogMsg("LIB330 CALLBACK (MINISEED): with pointer %p; %s %s-%s (sr: %d)\n", */
            /*         p, msp->station_name, msp->channel, msp->location, msp->rate); */

            if (msp->data_size != MSEED_DEFAULT_RECLEN) {
                LogMsg("UNEXPECTED MINISEED Record Size: %d\n", msp->data_size);
                break;
            }
            // get miniseed record

            if (!mseedUnpackRecord(&mseed, (UINT8 *)msp->data_address)) {
                LogMsg("ERROR Unpacking MINISEED Record\n");
                break;
            }

            /* LogMsg("Unpacked miniseed info: %s %s-%s (nsamp: %d, nsint: %lld, sr: %g)\n", */
            /*         mseed.hdr.staid, mseed.hdr.chnid, mseed.hdr.locid, mseed.hdr.nsamp, mseed.hdr.nsint, */
            /*         1e9 / (UINT32)mseed.hdr.nsint ); */

            FlushRecord((UINT8 *)msp->data_address);

            break;
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

	/* LogMsg("LIB330 CALLBACK (STATE): state_type=%u info=%u : %s\n", state->state_type, state->info, */
            /* lib_get_statestr(lib_get_state(state->context, &liberr, &opstat), &statestr)); */

    /* PrintLib330Topstat(&opstat); */

}

void isi330_msg_callback(pointer p)
{
    tmsg_call *msg;
    msg = (tmsg_call *)p;
    string95 codestr;

    /* LogMsg("LIB330 CALLBACK (MSG): # %u [code=%d]: %s%s\n", msg->msgcount, msg->code, lib_get_msg(msg->code, &codestr),  msg->suffix); */
}
