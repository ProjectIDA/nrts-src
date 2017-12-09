/*======================================================================
 *
 *  Aquire data from Q330 using Quanterra Lib330 library
 *
 *=+===================================================================*/
#include "isi330.h"
// #include "inttypes.h"

#define MY_MOD_ID ISI330_MOD_MAIN

// static BOOL Debug = FALSE;


int main(int argc, char **argv) {

    tcontext ct;
    enum tliberr liberr;
    enum tlibstate libstate;
    topstat opstat;
    string63 statestr;
    // tpar_create tpc;
    ISI330_CONFIG *cfg;



    cfg = init(argv[0], argc, argv);

    if (cfg == NULL) {
        fprintf(stderr, "ERROR: could not construct tpar_create struct\n");
        help(argv[0]);
    }

    /* PrintISI330Config(cfg); */

    /* create station thread context */

    lib_create_context(&ct, cfg->tpc);
    if (cfg->tpc->resp_err != 0) {
        perror("lib_create_context");
        exit(MY_MOD_ID + 1);
    }

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


    sleep(10000000);

}
