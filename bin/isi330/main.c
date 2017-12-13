/*======================================================================
 *
 *  Aquire data from Q330 using Quanterra Lib330 library
 *
 *=+===================================================================*/
#include "isi330.h"

#define MY_MOD_ID ISI330_MOD_MAIN

static BOOL Debug = FALSE;

void MainThread(MainThreadParams *cmdline)
{
    ISI330_CONFIG *cfg;
    static INT32 status;

    cfg = init(cmdline->myname, cmdline->argc, cmdline->argv);
    while (1) {
        if ((status = ExitStatus()) == 0) {
            sleep(1);
        } else {
//          LogMsg(LOG_DEBUG, "shutdown initiated: exit flag = %ld", status);
            GracefulExit(status);
        }
    }
}



int main(int argc, char **argv) {


    MainThreadParams cmdline;

    cmdline.argc   = argc;
    cmdline.argv   = argv;
    cmdline.myname = argv[0];

    MainThread(&cmdline);


    tcontext ct;
    enum tliberr liberr;
    enum tlibstate libstate, newstate;
    topstat opstat;
    string63 statestr;
    ISI330_CONFIG *cfg;
    string95 msgbuf;
    INT32 status = 0;

    cfg = init(argv[0], argc, argv);

/* Start signal handling thread */

    StartSignalHandler();

/* Initialize the exit facility */

    InitExit(cfg);

    if (cfg == NULL) {
        fprintf(stderr, "ERROR: could not construct tpar_create struct\n");
        help(argv[0]);
    }

    /* PrintISI330Config(cfg); */

    /* create station thread context */
    lib_create_context(&ct, cfg->tpc);
    strcpy(msgbuf, "COMPLETED: lib_create_context\n");
    lib_msg_add(ct, LIBMSG_USER, 0, &msgbuf);
    if (cfg->tpc->resp_err != 0) {
        perror("lib_create_context");
        exit(MY_MOD_ID + 1);
    }

    cfg->ct = &ct;

    libstate = lib_get_state(ct, &liberr, &opstat);
    if (liberr != LIBERR_NOERR) {
        PrintLib330Tliberr(liberr);
    } else {
        printf("libstate: %s\n", lib_get_statestr(libstate, &statestr));
    }
    /* ping q330 to see if can register with q330 */

    /* liberr = lib_unregistered_ping(ct, cfg->tpr); */
    /* if (liberr != LIBERR_NOERR) { */
    /*     PrintLib330Tliberr(liberr); */
    /* } else { */
    /*     printf("lib_unregistered_ping successful\n"); */
    /* } */

    liberr = lib_register(ct, &cfg->tpr);
    if (liberr != LIBERR_NOERR) {
        PrintLib330Tliberr(liberr);
    } else {
        printf("lib_register successful\n");
    }


    libstate = lib_get_state(ct, &liberr, &opstat);
    while (1) {
        if ((status = ExitStatus()) == 0) {
            sleep(1);

            newstate = lib_get_state(ct, &liberr, &opstat);
            if (newstate != libstate) {
                libstate = newstate;
                printf("NEW State [%d]: %s\n", (int)libstate, lib_get_statestr(libstate, &statestr));
                switch (libstate) {
                    case LIBSTATE_IDLE :
                    case LIBSTATE_TERM :
                    case LIBSTATE_PING :
                    case LIBSTATE_CONN :
                    case LIBSTATE_ANNC :
                    case LIBSTATE_REG :
                    case LIBSTATE_READCFG :
                    case LIBSTATE_READTOK :
                    case LIBSTATE_DECTOK :
                    case LIBSTATE_RUNWAIT :
                    case LIBSTATE_RUN :
                    case LIBSTATE_DEALLOC :
                    case LIBSTATE_DEREG :
                    case LIBSTATE_WAIT :
                    default:
                        break;
                }
            }
        } else {
            LogMsg("shutdown initiated: exit flag = %ld", status);
            GracefulExit(status);
        }
    }

}
