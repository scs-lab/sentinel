//
// Created by mani on 9/14/2020.
//

#ifndef SENTINEL_COMMON_DATA_STRUCTURES_H
#define SENTINEL_COMMON_DATA_STRUCTURES_H


#include <basket/common/data_structures.h>
#include <cstdint>
#include <rpc/msgpack.hpp>

#include <basket.h>
#include <common/data_structure.h>
#include <sentinel/common/enumerations.h>
#include <vector>

typedef struct Event: public Data{

    OperationType type_;
    /*Define the default, copy and move constructor*/
    Event(): Data(),type_(){}

    Event(const Event &other): Data(other),type_(other.type_){}
    Event(Event &other): Data(other),type_(other.type_){}
    /*Define Assignment Operator*/
    Event &operator=(const Event &other){
        Data::operator=(other);
        type_ = other.type_;
        return *this;
    }
}Event;

typedef struct Task{

    std::vector<std::shared_ptr<Task>> links;

    Task():links(){}

    virtual void Execute(){
        printf("Test task's execute function....\n");
    }
}Task;

typedef struct SourceTask: public Task{
    SourceTask():Task(){}

    virtual void Execute(){
        printf("Test SourceTask's execute function....\n");
    }
}SourceTask;

typedef struct KeyByTask: public Task{
    KeyByTask():Task(){}

    virtual void Execute(){
        printf("Test KeyByTask's execute function....\n");
    }
}KeyByTask;

typedef struct SinkTask: public Task{
    SinkTask():Task(){}

    virtual void Execute(){
        printf("Test SinkTask's execute function....\n");
    }
}SinkTask;

typedef struct Job{

    std::shared_ptr<Task> source_;

    Job(): source_(){}
    Job(const Job &other): source_(other.source_) {}
    Job(Job &other): source_(other.source_) {}
    /*Define Assignment Operator*/
    Job &operator=(const Job &other){
        source_ = other.source_;
        return *this;
    }
    std::shared_ptr<Task> GetTask(uint32_t task_id_){
        printf("Begin to create Task....\n");
        return std::make_shared<Task>();
    }

    uint32_t GetNextTaskId(uint32_t task_id_){
        printf("Test Job's test function....\n");
        return task_id_ + 1;
    }

    uint32_t GetCollectorId(){
        printf("Give me collector....\n");
        return 1;
    }

}Job;


typedef struct ResourceAllocation {
    uint16_t job_id_;
    uint16_t num_nodes_;
    uint16_t num_procs_per_node;
    uint16_t num_threads_per_proc;

    ResourceAllocation(uint16_t job_id, uint16_t num_nodes, uint16_t num_procs_per_node, uint16_t num_threads_per_proc): job_id_(job_id),num_nodes_(num_nodes), num_procs_per_node(num_procs_per_node), num_threads_per_proc(num_threads_per_proc){}

    /*Define the default, copy and move constructor*/
    ResourceAllocation(): job_id_(0),num_nodes_(0), num_procs_per_node(0), num_threads_per_proc(){}
    ResourceAllocation(const ResourceAllocation &other): job_id_(other.job_id_),num_nodes_(other.num_nodes_), num_procs_per_node(other.num_procs_per_node), num_threads_per_proc(other.num_threads_per_proc){}
    ResourceAllocation(ResourceAllocation &other): job_id_(other.job_id_) ,num_nodes_(other.num_nodes_), num_procs_per_node(other.num_procs_per_node), num_threads_per_proc(other.num_threads_per_proc){}

    /*Define Assignment Operator*/
    ResourceAllocation &operator=(const ResourceAllocation &other){
        num_nodes_ = other.num_nodes_;
        num_procs_per_node = other.num_procs_per_node;
        num_threads_per_proc = other.num_threads_per_proc;
        job_id_ = other.job_id_;
        return *this;
    }
} ResourceAllocation;

typedef struct WorkerManagerStats {

    double thrpt_kops_;
    uint32_t num_tasks_exec_;
    uint32_t num_tasks_queued_;
    WorkerManagerStats():thrpt_kops_(0),num_tasks_exec_(0),num_tasks_queued_(0){}
    WorkerManagerStats(double epoch_time, int num_tasks_assigned, int num_tasks_queued) {
        num_tasks_exec_ = num_tasks_assigned - num_tasks_queued;
        num_tasks_queued_ = num_tasks_queued;
        thrpt_kops_ = num_tasks_exec_ / epoch_time;
    }
    /*Define the default, copy and move constructor*/
    WorkerManagerStats(const WorkerManagerStats &other): thrpt_kops_(other.thrpt_kops_), num_tasks_exec_(other.num_tasks_exec_), num_tasks_queued_(other.num_tasks_queued_) {}
    WorkerManagerStats(WorkerManagerStats &other):  thrpt_kops_(other.thrpt_kops_), num_tasks_exec_(other.num_tasks_exec_), num_tasks_queued_(other.num_tasks_queued_) {}
    /*Define Assignment Operator*/
    WorkerManagerStats &operator=(const WorkerManagerStats &other)= default;

    bool operator<(const WorkerManagerStats &other) const{
        return (num_tasks_queued_ < other.num_tasks_queued_);
    }
} WorkerManagerStats;
namespace clmdep_msgpack {
    MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
        namespace mv1 = clmdep_msgpack::v1;

        namespace adaptor {
            template<>
            struct convert<Event> {
                mv1::object const &operator()(mv1::object const &o, Event &input) const {
                    input.id_ = o.via.array.ptr[0].as<CharStruct>();
                    input.position_ = o.via.array.ptr[1].as<size_t>();
                    auto data = o.via.array.ptr[2].as<std::string>();
                    input.data_size_ = o.via.array.ptr[3].as<size_t>();
                    if (!data.empty()) {
                        input.buffer_ = static_cast<char *>(malloc(input.data_size_));
                        memcpy(input.buffer_, data.data(), input.data_size_);
                    }
                    input.storage_index_ = o.via.array.ptr[4].as<uint16_t>();
                    return o;
                }
            };

            template<>
            struct pack<Event> {
                template<typename Stream>
                packer<Stream> &operator()(mv1::packer<Stream> &o, Event const &input) const {
                    o.pack_array(5);
                    o.pack(input.id_);
                    o.pack(input.position_);
                    if (input.buffer_ == NULL) o.pack(std::string());
                    else {
                        o.pack(std::string(input.buffer_, input.data_size_));
                    }
                    o.pack(input.data_size_);
                    o.pack(input.storage_index_);
                    return o;
                }
            };

            template<>
            struct object_with_zone<Event> {
                void operator()(mv1::object::with_zone &o, Event const &input) const {
                    o.type = type::ARRAY;
                    o.via.array.size = 5;
                    o.via.array.ptr = static_cast<clmdep_msgpack::object *>(o.zone.allocate_align(
                            sizeof(mv1::object) * o.via.array.size, MSGPACK_ZONE_ALIGNOF(mv1::object)));
                    o.via.array.ptr[0] = mv1::object(input.id_, o.zone);
                    o.via.array.ptr[1] = mv1::object(input.position_, o.zone);
                    if (input.buffer_ == NULL) o.via.array.ptr[2] = mv1::object(std::string(), o.zone);
                    else {
                        o.via.array.ptr[2] = mv1::object(std::string(input.buffer_, input.data_size_), o.zone);
                        free(input.buffer_);
                    }
                    o.via.array.ptr[3] = mv1::object(input.data_size_, o.zone);
                    o.via.array.ptr[4] = mv1::object(input.storage_index_, o.zone);
                }
            };

            template<>
            struct convert<WorkerManagerStats> {
                mv1::object const &operator()(mv1::object const &o, WorkerManagerStats &input) const {
                    input.thrpt_kops_ = o.via.array.ptr[0].as<double>();
                    input.num_tasks_exec_ = o.via.array.ptr[1].as<uint32_t>();
                    input.num_tasks_queued_ = o.via.array.ptr[2].as<uint32_t>();
                    return o;
                }
            };

            template<>
            struct pack<WorkerManagerStats> {
                template<typename Stream>
                packer<Stream> &operator()(mv1::packer<Stream> &o, WorkerManagerStats const &input) const {
                    o.pack_array(3);
                    o.pack(input.thrpt_kops_);
                    o.pack(input.num_tasks_exec_);
                    o.pack(input.num_tasks_queued_);
                    return o;
                }
            };

            template<>
            struct object_with_zone<WorkerManagerStats> {
                void operator()(mv1::object::with_zone &o, WorkerManagerStats const &input) const {
                    o.type = type::ARRAY;
                    o.via.array.size = 3;
                    o.via.array.ptr = static_cast<clmdep_msgpack::object *>(o.zone.allocate_align(
                            sizeof(mv1::object) * o.via.array.size, MSGPACK_ZONE_ALIGNOF(mv1::object)));
                    o.via.array.ptr[0] = mv1::object(input.thrpt_kops_, o.zone);
                    o.via.array.ptr[1] = mv1::object(input.num_tasks_exec_, o.zone);
                    o.via.array.ptr[2] = mv1::object(input.num_tasks_queued_, o.zone);
                }
            };

            template<>
            struct convert<ResourceAllocation> {
                mv1::object const &operator()(mv1::object const &o, ResourceAllocation &input) const {
                    input.num_nodes_ = o.via.array.ptr[0].as<uint16_t>();
                    input.num_procs_per_node = o.via.array.ptr[1].as<uint16_t>();
                    input.num_threads_per_proc = o.via.array.ptr[2].as<uint16_t>();
                    return o;
                }
            };

            template<>
            struct pack<ResourceAllocation> {
                template<typename Stream>
                packer<Stream> &operator()(mv1::packer<Stream> &o, ResourceAllocation const &input) const {
                    o.pack_array(3);
                    o.pack(input.num_nodes_);
                    o.pack(input.num_procs_per_node);
                    o.pack(input.num_threads_per_proc);
                    return o;
                }
            };

            template<>
            struct object_with_zone<ResourceAllocation> {
                void operator()(mv1::object::with_zone &o, ResourceAllocation const &input) const {
                    o.type = type::ARRAY;
                    o.via.array.size = 3;
                    o.via.array.ptr = static_cast<clmdep_msgpack::object *>(o.zone.allocate_align(
                            sizeof(mv1::object) * o.via.array.size, MSGPACK_ZONE_ALIGNOF(mv1::object)));
                    o.via.array.ptr[0] = mv1::object(input.num_nodes_, o.zone);
                    o.via.array.ptr[1] = mv1::object(input.num_procs_per_node, o.zone);
                    o.via.array.ptr[2] = mv1::object(input.num_threads_per_proc, o.zone);
                }
            };

            template<>
            struct convert<Task> {
                mv1::object const &operator()(mv1::object const &o, Task &input) const {
                    input.links = o.via.array.ptr[0].as<std::vector<std::shared_ptr<Task>>>();
                    return o;
                }
            };

            template<>
            struct pack<Task>{
                template<typename Stream>
                packer <Stream> &operator()(mv1::packer <Stream> &o, Task const &input) const {
                    o.pack_array(1);
                    o.pack(input.links);
                    return o;
                }
            };

            template<>
            struct object_with_zone<Task> {
                void operator()(mv1::object::with_zone &o, Task const &input) const {
                    o.type = type::ARRAY;
                    o.via.array.size = 1;
                    o.via.array.ptr = static_cast<clmdep_msgpack::object *>(o.zone.allocate_align(
                            sizeof(mv1::object) * o.via.array.size, MSGPACK_ZONE_ALIGNOF(mv1::object)));
                    o.via.array.ptr[0] = mv1::object(input.links, o.zone);
                }
            };

            template<>
            struct convert<SourceTask> {
                mv1::object const &operator()(mv1::object const &o, SourceTask &input) const {
                    input.links = o.via.array.ptr[0].as<std::vector<std::shared_ptr<Task>>>();
                    return o;
                }
            };

            template<>
            struct pack<SourceTask>{
                template<typename Stream>
                packer <Stream> &operator()(mv1::packer <Stream> &o, SourceTask const &input) const {
                    o.pack_array(1);
                    o.pack(input.links);
                    return o;
                }
            };

            template<>
            struct object_with_zone<SourceTask> {
                void operator()(mv1::object::with_zone &o, SourceTask const &input) const {
                    o.type = type::ARRAY;
                    o.via.array.size = 1;
                    o.via.array.ptr = static_cast<clmdep_msgpack::object *>(o.zone.allocate_align(
                            sizeof(mv1::object) * o.via.array.size, MSGPACK_ZONE_ALIGNOF(mv1::object)));
                    o.via.array.ptr[0] = mv1::object(input.links, o.zone);
                }
            };

            template<>
            struct convert<KeyByTask> {
                mv1::object const &operator()(mv1::object const &o, KeyByTask &input) const {
                    input.links = o.via.array.ptr[0].as<std::vector<std::shared_ptr<Task>>>();
                    return o;
                }
            };

            template<>
            struct pack<KeyByTask>{
                template<typename Stream>
                packer <Stream> &operator()(mv1::packer <Stream> &o, KeyByTask const &input) const {
                    o.pack_array(1);
                    o.pack(input.links);
                    return o;
                }
            };

            template<>
            struct object_with_zone<KeyByTask> {
                void operator()(mv1::object::with_zone &o, KeyByTask const &input) const {
                    o.type = type::ARRAY;
                    o.via.array.size = 1;
                    o.via.array.ptr = static_cast<clmdep_msgpack::object *>(o.zone.allocate_align(
                            sizeof(mv1::object) * o.via.array.size, MSGPACK_ZONE_ALIGNOF(mv1::object)));
                    o.via.array.ptr[0] = mv1::object(input.links, o.zone);
                }
            };

            template<>
            struct convert<SinkTask> {
                mv1::object const &operator()(mv1::object const &o, SinkTask &input) const {
                    input.links = o.via.array.ptr[0].as<std::vector<std::shared_ptr<Task>>>();
                    return o;
                }
            };

            template<>
            struct pack<SinkTask>{
                template<typename Stream>
                packer <Stream> &operator()(mv1::packer <Stream> &o, SinkTask const &input) const {
                    o.pack_array(1);
                    o.pack(input.links);
                    return o;
                }
            };

            template<>
            struct object_with_zone<SinkTask> {
                void operator()(mv1::object::with_zone &o, SinkTask const &input) const {
                    o.type = type::ARRAY;
                    o.via.array.size = 1;
                    o.via.array.ptr = static_cast<clmdep_msgpack::object *>(o.zone.allocate_align(
                            sizeof(mv1::object) * o.via.array.size, MSGPACK_ZONE_ALIGNOF(mv1::object)));
                    o.via.array.ptr[0] = mv1::object(input.links, o.zone);
                }
            };

        }  // namespace adaptor
    }
}  // namespace clmdep_msgpack

std::ostream &operator<<(std::ostream &os, Data &data);

std::ostream &operator<<(std::ostream &os, Event &event);

#endif //SENTINEL_COMMON_DATA_STRUCTURES_H
