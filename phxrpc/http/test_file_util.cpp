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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "phxrpc/file/file_utils.h"
#include "phxrpc/file/opt_map.h"

using namespace phxrpc;

void showUsage(const char * program) {
    printf("\n%s [-h host] [-p port] [-r POST|GET] [-u URI] [-f file] [-v]\n", program);

    printf("\t-f the file for POST body, only need for POST\n");
    printf("\t-v show this usage\n");
    printf("\n");

    exit(0);
}

int main(int argc, char * argv[]) {
    assert(sigset(SIGPIPE, SIG_IGN) != SIG_ERR);

    OptMap optMap("f:ov");

    if ((!optMap.Parse(argc, argv)) || optMap.Has('v'))
        showUsage(argv[0]);

    const char * file = optMap.Get('f');

    if (NULL == file) {
        printf("\nPlease specify the file!\n");
        showUsage(argv[0]);
    }

    std::string content = "\t因为网络协议设计本身有非常多的功能也会有比较大的字段，相关的代码可能会有数千行，\n"; 
    //+ "\t这一块不能通过手写方式完成。可以通过XML描述，再通过工具自动生成所有的代码，这是微信获得快速开发的一个重要的点。 Svrkit —— Client/Server自动代码生成框架:10分钟搭建内部服务器 随时添加新逻辑OssAgent——监控/统计框架：所见即所得的监控报表存\n" 
    //+ "\t组件——屏蔽容灾/扩容等复杂问题 \n";

    if (!FileUtils::WriteFile(file, &content)) {
        printf("Cannot write %s", file);
        exit(0);
    }

    return 0;
}

