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
    // tpar_create tpc;
    ISI330_CONFIG *cfg;

    char sta[ISI330_STATION_CODE_SIZE + 1];
    UINT16 dp;
    UINT64 sn;


    cfg = init(argv[0], argc, argv);

    if (cfg == NULL) {
        fprintf(stderr, "ERROR: could not construct tpar_create struct\n");
        help(argv[0]);
    }

    PrintISI330Config(cfg);

    /* create station thread context */

    lib_create_context(&ct, cfg->tpc);
    if (cfg->tpc->resp_err != 0) {
        perror("lib_create_context");
        exit(MY_MOD_ID + 1);
    }

    /* ping q330 to see if can register with q330 */

    lib_unregistered_ping(ct, cfg->tpr);
    lib_register(ct, cfg->tpr);

    printf("bye bye \n");
    sleep(100);

}
