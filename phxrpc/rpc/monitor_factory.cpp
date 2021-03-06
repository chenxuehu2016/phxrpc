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

#include <memory>
#include <stdlib.h>

#include "monitor_factory.h"

namespace phxrpc {

static MonitorFactory * g_monitor_factory_ = NULL;

MonitorFactory :: MonitorFactory() {
}

MonitorFactory :: ~MonitorFactory() {
}

void MonitorFactory :: SetFactory( MonitorFactory * factory ) {
    g_monitor_factory_ = factory;
}

MonitorFactory * MonitorFactory :: GetFactory() {
    static MonitorFactory monitor_factory;
    if ( !g_monitor_factory_ ) {
        return &monitor_factory;
    }
    return g_monitor_factory_;
}

ClientMonitorPtr MonitorFactory :: CreateClientMonitor( const char * package_name) {
    return ClientMonitorPtr( new ClientMonitor(package_name) );
}

ServerMonitorPtr MonitorFactory :: CreateServerMonitor( const char * package_name, const char* log_dir ) {
    return ServerMonitorPtr( new ServerMonitor(package_name, log_dir) );
}

}
