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

#include "server_monitor.h"
#include "phxrpc/file.h"

#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <sstream>

namespace phxrpc {

//ServerMonitor begin
ServerMonitor::ServerMonitor(const char *package_name, const char* log_dir ) :
    package_name(package_name), log_dir(log_dir) {
    //accept
    this->accept = 0;
    this->accept_fail = 0;
    this->fast_reject_accept = 0;

    //io
    this->read_error = 0;
    this->send_error = 0;
    this->out_of_queue = 0;
    this->fast_reject_after_read = 0;
    this->queue_delay = 0;
    this->send_bytes = 0;
    this->recv_bytes = 0;
    this->waitin_queue_cost = 0;
    this->waitout_queue_cost = 0;

    //worker
    this->request_qps = 0;
    this->response_qps = 0;
    this->request_cost_ms = 0;
    this->wroker_queue_timeout_count = 0;

    this->svr_call = {{}};


ServerMonitor :: ~ServerMonitor() {
}

void ServerMonitor :: writeToFile() {
    std::ostringstream content;

    //accept
    content << "accept = " << accept <<"\n";
    content << "accept_fail = " << accept_fail <<"\n";
    content << "fast_reject_accept = " << fast_reject_accept <<"\n";

    //io
    content << "read_error = " << read_error <<"\n";
    content << "send_error = " << send_error <<"\n";
    content << "out_of_queue = " << out_of_queue <<"\n";
    content << "queue_delay = " << queue_delay <<"\n";
    content << "fast_reject_after_read = " << fast_reject_after_read <<"\n";
    content << "send_bytes = " << send_bytes <<"\n";
    content << "recv_bytes = " << recv_bytes <<"\n";
    content << "waitin_queue_cost = " << waitin_queue_cost <<"\n";
    content << "waitout_queue_cost = " << waitout_queue_cost <<"\n";

    //worker
    content << "request_qps = " << request_qps <<"\n";
    content << "response_qps = " << response_qps <<"\n";
    content << "request_cost_ms = " << request_cost_ms <<"\n";
    content << "wroker_queue_timeout_count = " << wroker_queue_timeout_count <<"\n";

    content << "svr_call = ";
   
    for (auto it = this->svr_call.begin(); it != this->svr_call.end(); ++it) { // calls a_map.begin() and a_map.end()
        if (it->second > 0) {
            content << it->first << ":" << it->second << ',';
        }
    }
    content << "\n";

    char logPath[256];
    snprintf( logPath, sizeof( logPath ), "%s/%s", log_dir, package_name);

    std::string str(content.str());
    FileUtils::WriteFile(logPath, &str);
}

//accept
void ServerMonitor :: Accept( int count) {
    if (count <= 0) {
        return;
    }
    this->accept += count;
    this->writeToFile();
}

void ServerMonitor :: AcceptFail( int count ) {
    if (count <= 0) {
        return;
    }
    this->accept_fail += count;
}

void ServerMonitor :: RequestCount( int count ) {
    //if (count <= 0) {
    //    return;
    //}

    this->request_qps = count;
}

void ServerMonitor :: ResponseCount( int count ) {
    //if (count <= 0) {
    //    return;
    //}
    this->response_qps = count;
}

void ServerMonitor :: SendBytes( size_t bytes ) {
    if (bytes <= 0) {
        return;
    }
    this->send_bytes += bytes;
}

void ServerMonitor :: RecvBytes( size_t bytes ) {
    if (bytes <= 0) {
        return;
    }
    this->recv_bytes += bytes;
}

void ServerMonitor :: RequestCost( uint64_t cost_ms ) {
    if (cost_ms <= 0) {
        return;
    }
    this->request_cost_ms  = cost_ms;
}

void ServerMonitor :: ReadError( int count ) {
    if (count <= 0) {
        return;
    }
    this->read_error += count;
}

void ServerMonitor :: SendError( int count ) {
    if (count <= 0) {
        return;
    }
    this->send_error += count;
}

void ServerMonitor :: OutOfQueue( int count ) {
    if (count <= 0) {
        return;
    }
    this->out_of_queue += count;
}

void ServerMonitor :: QueueDelay( uint64_t cost_ms ) {
    if (cost_ms <= 0) {
        return;
    }
    this->queue_delay += cost_ms;
}

void ServerMonitor :: FastRejectAfterAccept( int count ) {
    if (count <= 0) {
        return;
    }
    this->fast_reject_accept += count;
}
    
void ServerMonitor :: FastRejectAfterRead( int count ) {
    if (count <= 0) {
        return;
    }
    this->fast_reject_after_read += count;
}

void ServerMonitor :: WrokerInQueueTimeout( int count ) {
    if (count <= 0) {
        return;
    }
    this->wroker_queue_timeout_count += count;
}

void ServerMonitor :: WaitInInQueue( uint64_t cost_ms ) {
    if (cost_ms <= 0) {
        return;
    }
    this->waitin_queue_cost += cost_ms;
}

void ServerMonitor :: WaitInOutQueue( uint64_t cost_ms ) {
    if (cost_ms <= 0) {
        return;
    }
    this->waitout_queue_cost += cost_ms;
}


void ServerMonitor :: SvrCall( int cmdid, const char * method_name, int count ) {
    if (count <= 0) {
        return;
    }
    auto search = this->svr_call.find(method_name);
    if(search != this->svr_call.end()) {
        search->second++;
        std::cout << "Found " << search->first << " " << search->second << '\n';
    }
    else {
        this->svr_call.insert(std::pair<const char *, int>(method_name, 1));
        std::cout << "Not found\n";
    }
}

//ServerMonitor end

}

