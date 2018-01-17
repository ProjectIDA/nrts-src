/*======================================================================
 *
 *  Callbacks provided to the Quanterra lib330 library
 *
 *=+===================================================================*/
#include <libclient.h>
#include "isi330.h"

static MSEED_HANDLE *mshandle = NULL;

void SetMSEEDHandle(MSEED_HANDLE *h) {
    mshandle = h;
}

void mseed_callback(void *unused, MSEED_PACKED *packed)
{
    // MSEED_RECORD mseed;

    // if (!mseedUnpackRecord(&mseed, (UINT8 *)packed)) {
    //     LogMsg("ERROR Unpacking MINISEED Record\n");
    //     break;
    // }

    FlushRecord((UINT8 *)packed);

    free(packed); /* don't forget to free! */
}


void isi330_one_second_callback(pointer p)
{
    static char *fid = "isi33_one_second_callback";
    tonesec_call *ponesec;
    MSEED_RECORD mseed;

    /* LogMsg("%s: called\n", fid); */

    if ((ponesec = (tonesec_call *)p) == NULL) {
        LogMsg("%s: NULL pointer received.\n", fid);
        return;
    }

    OneSecPacketToMSEED_RECORD(mshandle, &mseed, ponesec);

    if (!mseedAddRecordToHandle(mshandle, &mseed)) {
        LogMsg("*** ERROR *** %s: mseedAddRecordToHandle: %s", fid, strerror(errno));
        return;
    }



    /* LogMsg("%s:         total_size: %u\n", fid, ponesec->total_size); */
    /* LogMsg("%s:       station_name: %s\n", fid, ponesec->station_name); */
    /* LogMsg("%s:           location: %s\n", fid, ponesec->location); */
    /* LogMsg("%s:        chan_number: %hhu\n", fid, ponesec->chan_number); */
    /* LogMsg("%s:            channel: %s\n", fid, ponesec->channel); */
    /* LogMsg("%s:               rate: %d\n", fid, ponesec->rate); */
    /* LogMsg("%s:         cl_session: %lu\n", fid, ponesec->cl_session); */
    /* LogMsg("%s:          cl_offset: %lg\n", fid, ponesec->cl_offset); */
    /* LogMsg("%s:          timestamp: %f\n", fid, ponesec->timestamp); */
    /* LogMsg("%s:        filter_bits: %hu\n", fid, ponesec->filter_bits); */
    /* LogMsg("%s:          qual_perc: %hu\n", fid, ponesec->qual_perc); */
    /* LogMsg("%s:     activity_flags: %hu\n", fid, ponesec->activity_flags); */
    /* LogMsg("%s:           io_flags: %hu\n", fid, ponesec->io_flags); */
    /* LogMsg("%s: data_quality_flags: %hu\n", fid, ponesec->data_quality_flags); */
    /* LogMsg("%s:        src_channel: %u\n", fid, ponesec->src_channel); */
    /* LogMsg("%s:        src_subchan: %u\n", fid, ponesec->src_subchan); */
}

void isi330_miniseed_callback(pointer p)
{
    tminiseed_call *msp;
    char buf[256];
    MSEED_RECORD mseed;
    char ida1012[IDA10_FIXEDRECLEN];

    msp = (tminiseed_call *)p;

    if (msp->chan_number == 0xFF) LogMsg("\nDP STATISTICS\n");

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

            /* if (mseed512ToIDA1012(mseed, ida1012) == NULL) { */
            /*     fprintf(stderr, "%s: mseed512ToIDA1012: %s\n", argv[0], strerror(errno)); */
            /*     exit(1); */
            /* } */
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
