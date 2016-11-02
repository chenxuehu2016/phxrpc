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

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <sstream>

#include "name_render.h"
#include "syntax_tree.h"

#include "code_utils.h"

using namespace phxrpc;

NameRender::NameRender(const char * prefix) {
    memset(prefix_, 0, sizeof(prefix_));
    strncpy(prefix_, prefix, sizeof(prefix_) - 1);
}

NameRender::~NameRender() {
}

const char * NameRender::GetPrefix(char * dest, int size) {
    snprintf(dest, size, "%s", prefix_);

    ToUpper(dest);

    return dest;
}

const char * NameRender::GetMessageClasname(const char * type, char * name, int size) {
    std::string tmp = type;

    phxrpc::StrReplaceAll(&tmp, ".", "::");

    snprintf(name, size, "%s", tmp.c_str());

    return name;
}

const char * NameRender::GetFuncCppDeclaration(const char *type, char * name, int size) {
    // 参考 google/protobuf/wrappers.proto
    //https://developers.google.com/protocol-buffers/docs/proto#scalar
    char type_name[128] = { 0 };
    std::string result;

    GetMessageClasname(type, type_name, sizeof(type_name));
    if (0 == strcmp("google::protobuf::Empty", type_name)) {
        // nothing
    } else if (0 == strcmp("google::protobuf::DoubleValue", type_name)) {
        result.append("double");
    } else if (0 == strcmp("google::protobuf::FloatValue", type_name)) {
        result.append("float");
    } else if (0 == strcmp("google::protobuf::Int64Value", type_name)) {
        result.append("int64");
    } else if (0 == strcmp("google::protobuf::UInt64Value", type_name)) {
        result.append("uint64");
    } else if (0 == strcmp("google::protobuf::Int32Value", type_name)) {
        result.append("int32");
    } else if (0 == strcmp("google::protobuf::UInt32Value", type_name)) {
        result.append("uint32");
    } else if (0 == strcmp("google::protobuf::BoolValue", type_name)) {
        result.append("bool");
    } else if (0 == strcmp("google::protobuf::StringValue", type_name)) {
        result.append("std::string");
    } else if (0 == strcmp("google::protobuf::BytesValue", type_name)) {
        result.append("std::string");
    } else {
        result.append(type);
    }

    snprintf(name, size, "%s", result.c_str());

    return name;
}

const char * NameRender::GetMessageFileName(const char *name, char * dest, int size) {
    snprintf(dest, size, "%s%s", prefix_, name);

    char * pos = strrchr(dest, '.');
    if (NULL != pos)
        *pos = '\0';

    strncat(dest, ".pb", size);

    ToLower(dest);

    return dest;
}

char * NameRender::ToLower(char *s) {
    char *ret = s;

    for (; *s != '\0'; ++s)
        *s = tolower(*s);

    return ret;
}

char * NameRender::ToUpper(char *s) {
    char * ret = s;

    for (; *s != '\0'; s++)
        *s = toupper(*s);

    return ret;
}

const char * NameRender::GetStubClasname(const char * name, char *dest, int size) {
    snprintf(dest, size, "%s%c%sStub", prefix_, toupper(*name), name + 1);

    return dest;
}

const char * NameRender::GetStubFileName(const char *name, char * dest, int size) {
    snprintf(dest, size, "phxrpc_%s%c%s_stub", prefix_, toupper(*name), name + 1);

    ToLower(dest);

    return dest;
}

const char * NameRender::GetClientClasname(const char *name, char * dest, int size) {
    snprintf(dest, size, "%s%c%sClient", prefix_, toupper(*name), name + 1);

    return dest;
}

const char * NameRender::GetClientClasnameLower(const char *name, char * dest, int size) {
    snprintf(dest, size, "%s%c%sClient", prefix_, toupper(*name), name + 1);

    ToLower(dest);

    return dest;
}

const char * NameRender::GetClientFileName(const char *name, char * dest, int size) {
    snprintf(dest, size, "%s%c%s_client", prefix_, toupper(*name), name + 1);

    ToLower(dest);

    return dest;
}

const char * NameRender::GetClientEtcFileName(const char *name, char * dest, int size) {
    snprintf(dest, size, "%s%s_client.conf", prefix_, name);

    ToLower(dest);

    return dest;
}

const char * NameRender::GetServerConfigClasname(const char *name, char * dest, int size) {
    snprintf(dest, size, "%s%c%sServerConfig", prefix_, toupper(*name), name + 1);
    return dest;
}

const char * NameRender::GetServerConfigFileName(const char *name, char * dest, int size) {
    snprintf(dest, size, "%s%c%s_server_config", prefix_, toupper(*name), name + 1);

    ToLower(dest);

    return dest;
}

const char * NameRender::GetServerEtcFileName(const char *name, char * dest, int size) {
    snprintf(dest, size, "%s%s_server.conf", prefix_, name);

    ToLower(dest);

    return dest;
}

const char * NameRender::GetServerMainFileName(const char *name, char * dest, int size) {
    snprintf(dest, size, "%s%c%s_main", prefix_, toupper(*name), name + 1);

    ToLower(dest);

    return dest;
}

const char * NameRender::GetToolClasname(const char * name, char * dest, int size) {
    snprintf(dest, size, "%s%c%sTool", prefix_, toupper(*name), name + 1);

    return dest;
}

const char * NameRender::GetToolFileName(const char * name, char * dest, int size) {
    snprintf(dest, size, "phxrpc_%s%s_tool", prefix_, name);

    ToLower(dest);

    return dest;
}

const char * NameRender::GetToolImplClasname(const char * name, char * dest, int size) {
    snprintf(dest, size, "%s%c%sToolImpl", prefix_, toupper(*name), name + 1);

    return dest;
}

const char * NameRender::GetToolImplFileName(const char * name, char * dest, int size) {
    snprintf(dest, size, "%s%s_tool_impl", prefix_, name);

    ToLower(dest);

    return dest;
}

const char * NameRender::GetToolMainFileName(const char * name, char * dest, int size) {
    snprintf(dest, size, "%s%s_tool_main", prefix_, name);

    ToLower(dest);

    return dest;
}

void NameRender::GetCopyright(const char * tool_name, const char * proto_file, std::string * result, bool dont_edit,
                              const char * comment_prefix) {
    std::ostringstream tmp;

    tmp << comment_prefix << std::endl;
    tmp << comment_prefix << " Generated by " << tool_name << " from " << proto_file << std::endl;

    if (dont_edit) {
        tmp << comment_prefix << std::endl;
        tmp << comment_prefix << " Please DO NOT edit unless you know exactly what you are doing.\n";
    }
    tmp << comment_prefix << std::endl;

    *result = tmp.str();
}

const char * NameRender::GetServiceClasname(const char * name, char * dest, int size) {
    snprintf(dest, size, "%s%c%sService", prefix_, toupper(*name), name + 1);

    return dest;
}

const char * NameRender::GetServiceFileName(const char * name, char * dest, int size) {
    snprintf(dest, size, "phxrpc_%s%c%s_service", prefix_, toupper(*name), name + 1);

    ToLower(dest);

    return dest;
}

const char * NameRender::GetServiceImplClasname(const char * name, char * dest, int size) {
    snprintf(dest, size, "%s%c%sServiceImpl", prefix_, toupper(*name), name + 1);

    return dest;
}

const char * NameRender::GetServiceImplFileName(const char * name, char * dest, int size) {
    snprintf(dest, size, "%s%c%s_service_impl", prefix_, toupper(*name), name + 1);

    ToLower(dest);

    return dest;
}

const char * NameRender::GetServerHandlerClasname(const char * name, char * dest, int size) {
    snprintf(dest, size, "%s%c%sServerHandler", prefix_, toupper(*name), name + 1);

    return dest;
}

const char * NameRender::GetServerHandlerFileName(const char * name, char * dest, int size) {
    snprintf(dest, size, "%s%c%s_server_handler", prefix_, toupper(*name), name + 1);

    ToLower(dest);

    return dest;
}

const char * NameRender::GetDispatcherClasname(const char * name, char * dest, int size) {
    snprintf(dest, size, "%s%c%sDispatcher", prefix_, toupper(*name), name + 1);

    return dest;
}

const char * NameRender::GetDispatcherFileName(const char * name, char * dest, int size) {
    snprintf(dest, size, "phxrpc_%s%c%s_dispatcher", prefix_, toupper(*name), name + 1);

    ToLower(dest);

    return dest;
}

