//
// Created by mani on 9/14/2020.
//

#ifndef RHEA_CLIENT_H
#define RHEA_CLIENT_H

#include <mpi.h>
#include <basket/communication/rpc_lib.h>
#include <basket/communication/rpc_factory.h>
#include <sentinel/common/data_structures.h>
#include <sentinel/common/configuration_manager.h>

namespace sentinel::job_manager {
    class client {
    private:
        std::shared_ptr<RPC> rpc;
    public:
        client();

        bool SubmitJob(uint32_t jobId);
        bool UpdateWorkerManagerStats(uint32_t workerManagerId, WorkerManagerStats stats);
        WorkerManagerStats GetWorkerManagerStats(uint32_t workerManagerId);
        std::pair<uint32_t, uint32_t> GetNextNode(uint32_t currentTaskId);
        bool ChangeResourceAllocation(ResourceAllocation &resourceAllocation);

    };
}


#endif //RHEA_CLIENT_H