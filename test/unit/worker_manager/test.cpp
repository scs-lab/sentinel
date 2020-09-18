//
// Created by lukemartinlogan on 9/17/20.
//

#include <sentinel/worker_manager/client.h>

int main(int argc, char **argv)
{
    MPI_Init(&argc,&argv);
    if(argc > 1) SENTINEL_CONF->CONFIGURATION_FILE=argv[1];
    sentinel::worker_manager::Client client;
    for(int i = 0; i < 100; ++i) {
        client.AssignTask(0, 0);
    }
    usleep(1000000);
    client.FinalizeWorkerManager(0);
    MPI_Finalize();
    return 0;
}