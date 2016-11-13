/*
Tencent is pleased to support the open source community by making 
PhxRPC available.
Copyright (C) 2016 THL A29 Limited, a Tencent company. 
All rights reserved.

Licensed under the BSD 3-Clause License (the "License"); you may 
not use this file except in compliance with the License. You may 
obtain a copy of the License at

https://opensource.org/licenses/BSD-3-Clause

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or 
implied. See the License for the specific language governing 
permissions and limitations under the License.

See the AUTHORS file for names of contributors.
*/

#pragma once

#include <stdint.h>
#include <memory>
#include <sys/types.h>
#include <map>
// #include <pthread.h>

namespace phxrpc {

class ServerMonitor {
 public:
    //ServerMonitor();

    ServerMonitor(const char *package_name, const char* log_dir );

    virtual ~ServerMonitor();

    virtual void Accept( int count);

    virtual void AcceptFail( int count );

    virtual void RequestCount( int count );

    virtual void ResponseCount( int count );

    virtual void SendBytes( size_t bytes );

    virtual void RecvBytes( size_t bytes );

    virtual void RequestCost( uint64_t cost_ms );

    virtual void ReadError( int count );

    virtual void SendError( int count );

    virtual void OutOfQueue( int count );

    virtual void QueueDelay( uint64_t cost_ms );

    virtual void FastRejectAfterAccept( int count );

    virtual void FastRejectAfterRead( int count );

    virtual void WrokerInQueueTimeout( int count );

    virtual void WaitInInQueue( uint64_t cost_ms );

    virtual void WaitInOutQueue( uint64_t cost_ms );

    virtual void SvrCall( int cmdid, const char * method_name, int count );

private:
    const char *package_name;

    const char *log_dir;

    void writeToFile();

    //accept
    int accept;

    int accept_fail;

    int fast_reject_accept;

    //io

    int read_error;

    int send_error;

    int out_of_queue;

    int fast_reject_after_read;

    uint64_t queue_delay;

    size_t send_bytes;

    size_t recv_bytes;

    int waitin_queue_cost;

    int waitout_queue_cost;

    //worker
    int request_qps;

    int response_qps;

    int request_cost_ms;

    int wroker_queue_timeout_count;

    std::map<std::string, int> svr_call;
};

typedef std::shared_ptr<ServerMonitor> ServerMonitorPtr;
}

