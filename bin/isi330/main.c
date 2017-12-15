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
printf("done init-ing...\n");
    while (1) {
        sleep(1);
        // TODO
//        if ((status = ExitStatus()) == 0) {
//        } else {
//            LogMsg(LOG_INFO, "shutdown initiated: exit flag = %ld", status);
//            GracefulExit(status);
//        }
    }
}



int main(int argc, char **argv) {

printf("Top of Main...\n");
    MainThreadParams cmdline;

    cmdline.argc   = argc;
    cmdline.argv   = argv;
    cmdline.myname = argv[0];

printf("Go to mMainThread...\n");
    MainThread(&cmdline);

}
