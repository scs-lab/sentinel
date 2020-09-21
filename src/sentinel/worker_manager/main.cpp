//
// Created by lukemartinlogan on 9/16/20.
//

#include <basket/common/singleton.h>
#include <common/daemon.h>
#include <sentinel/common/configuration_manager.h>
#include "server.h"
#include <mpi.h>

int main(int argc, char **argv) {
    MPI_Init(&argc,&argv);
    if(argc > 1) SENTINEL_CONF->CONFIGURATION_FILE=argv[1];
    auto daemon = basket::Singleton<common::Daemon<sentinel::worker_manager::Server>>::GetInstance();
    daemon->Run();
    MPI_Finalize();
    return 0;
}