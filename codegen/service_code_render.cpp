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

#include "service_code_render.h"

#include "name_render.h"
#include "syntax_tree.h"

#include "code_utils.h"

using namespace phxrpc;

ServiceCodeRender::ServiceCodeRender(NameRender & name_render)
        : name_render_(name_render) {
}

ServiceCodeRender::~ServiceCodeRender() {
}

void ServiceCodeRender::GenerateServiceHpp(SyntaxTree * stree, FILE * write) {
    char filename[128] = { 0 };
    name_render_.GetServiceFileName(stree->GetName(), filename, sizeof(filename));

    std::string buffer;
    name_render_.GetCopyright("phxrpc_pb2service", stree->GetProtoFile(), &buffer);

    fprintf(write, "/* %s.h\n", filename);
    fprintf(write, "%s", buffer.c_str());
    fprintf(write, "*/\n");
    fprintf(write, "\n");

    fprintf(write, "#pragma once\n");

    fprintf(write, "\n");

    name_render_.GetMessageFileName(stree->GetProtoFile(), filename, sizeof(filename));
    fprintf(write, "#include \"%s.h\"\n", filename);

    fprintf(write, "\n");

    char clasname[128] = { 0 };
    name_render_.GetServiceClasname(stree->GetName(), clasname, sizeof(clasname));

    fprintf(write, "class %s\n", clasname);
    fprintf(write, "{\n");
    fprintf(write, "public:\n");
    fprintf(write, "    %s();\n", clasname);
    fprintf(write, "    virtual ~%s();\n", clasname);
    fprintf(write, "\n");

    SyntaxFuncVector * flist = stree->GetFuncList();
    SyntaxFuncVector::iterator fit = flist->begin();
    for (; flist->end() != fit; ++fit) {
        std::string buffer;
        GetServiceFuncDeclaration(stree, &(*fit), 1, 0, 1, &buffer);
        fprintf(write, "    virtual %s;\n", buffer.c_str());
        fprintf(write, "\n");
    }

    fprintf(write, "};\n");

    fprintf(write, "\n");
}

void ServiceCodeRender::GenerateServiceCpp(SyntaxTree * stree, FILE * write) {
    char filename[128] = { 0 };
    name_render_.GetServiceFileName(stree->GetName(), filename, sizeof(filename));

    std::string buffer;
    name_render_.GetCopyright("phxrpc_pb2service", stree->GetProtoFile(), &buffer);

    fprintf(write, "/* %s.cpp\n", filename);
    fprintf(write, "%s", buffer.c_str());
    fprintf(write, "*/\n");
    fprintf(write, "\n");

    fprintf(write, "#include \"%s.h\"\n", filename);

    name_render_.GetMessageFileName(stree->GetProtoFile(), filename, sizeof(filename));
    fprintf(write, "#include \"%s.h\"\n", filename);
    fprintf(write, "#include \"phxrpc/file.h\"\n");

    fprintf(write, "\n");

    char clasname[128] = { 0 };
    name_render_.GetServiceClasname(stree->GetName(), clasname, sizeof(clasname));

    fprintf(write, "%s :: %s()\n", clasname, clasname);
    fprintf(write, "{\n");
    fprintf(write, "}\n");
    fprintf(write, "\n");

    fprintf(write, "%s :: ~%s()\n", clasname, clasname);
    fprintf(write, "{\n");
    fprintf(write, "}\n");
    fprintf(write, "\n");

    SyntaxFuncVector * flist = stree->GetFuncList();
    SyntaxFuncVector::iterator fit = flist->begin();
    for (; flist->end() != fit; ++fit) {
        std::string buffer;
        GetServiceFuncDeclaration(stree, &(*fit), 0, 0, 0, &buffer);
        fprintf(write, "%s\n", buffer.c_str());
        fprintf(write, "{\n");
        fprintf(write, "    phxrpc::log( LOG_ERR, \"ERROR: %s unimplemented\" );\n", fit->GetName());
        fprintf(write, "    return -1;\n");
        fprintf(write, "}\n");
        fprintf(write, "\n");
    }

    fprintf(write, "\n");
}

void ServiceCodeRender::GenerateServiceImplHpp(SyntaxTree * stree, FILE * write) {
    char filename[128] = { 0 };
    name_render_.GetServiceImplFileName(stree->GetName(), filename, sizeof(filename));

    std::string buffer;
    name_render_.GetCopyright("phxrpc_pb2service", stree->GetProtoFile(), &buffer, false);

    fprintf(write, "/* %s.h\n", filename);
    fprintf(write, "%s", buffer.c_str());
    fprintf(write, "*/\n");
    fprintf(write, "\n");

    fprintf(write, "#pragma once\n");

    fprintf(write, "\n");

    name_render_.GetMessageFileName(stree->GetProtoFile(), filename, sizeof(filename));
    fprintf(write, "#include \"%s.h\"\n", filename);

    name_render_.GetServiceFileName(stree->GetName(), filename, sizeof(filename));
    fprintf(write, "#include \"%s.h\"\n", filename);

    name_render_.GetServerHandlerFileName(stree->GetName(), filename, sizeof(filename));
    fprintf(write, "#include \"%s.h\"\n", filename);

    fprintf(write, "\n");

    char clasname[128] = { 0 }, base_name[128] = { 0 }, config_name[128] = { 0 }, handler_name[128] = { 0 };
    name_render_.GetServiceClasname(stree->GetName(), base_name, sizeof(base_name));
    name_render_.GetServiceImplClasname(stree->GetName(), clasname, sizeof(clasname));
    name_render_.GetServerConfigClasname(stree->GetName(), config_name, sizeof(config_name));
    name_render_.GetServerHandlerClasname(stree->GetName(), handler_name, sizeof(handler_name));

    fprintf(write, "class %s;\n", config_name);
    fprintf(write, "class %s;\n", handler_name);
    fprintf(write, "\n");

    fprintf(write, "typedef struct tagServiceArgs {\n");
    fprintf(write, "    %s * config;\n", config_name);
    fprintf(write, "    %s * svr_handler_;\n", handler_name);
    fprintf(write, "    //You can add other arguments here and initiate in main().\n");
    fprintf(write, "}ServiceArgs_t;\n");
    fprintf(write, "\n");

    fprintf(write, "class %s : public %s\n", clasname, base_name);
    fprintf(write, "{\n");
    fprintf(write, "public:\n");
    fprintf(write, "    %s( ServiceArgs_t & args );\n", clasname);
    fprintf(write, "    virtual ~%s();\n", clasname);
    fprintf(write, "\n");

    SyntaxFuncVector * flist = stree->GetFuncList();
    SyntaxFuncVector::iterator fit = flist->begin();
    for (; flist->end() != fit; ++fit) {
        std::string buffer;
        GetServiceFuncDeclaration(stree, &(*fit), 1, 1, 1, &buffer);
        fprintf(write, "    virtual %s;\n", buffer.c_str());
        fprintf(write, "\n");
    }

    fprintf(write, "private:\n");
    fprintf(write, "    ServiceArgs_t & args_;\n" );
    fprintf(write, "    %s * svr_handler_;\n", handler_name);
	
    fprintf(write, "};\n");

    fprintf(write, "\n");
}

void ServiceCodeRender::GenerateServiceImplCpp(SyntaxTree * stree, FILE * write) {
    char filename[128] = { 0 }, config_file[128] = { 0 }, handler_file[128] = { 0 };
    name_render_.GetServiceImplFileName(stree->GetName(), filename, sizeof(filename));
    name_render_.GetServerConfigFileName(stree->GetName(), config_file, sizeof(config_file));
    name_render_.GetServerHandlerFileName(stree->GetName(), handler_file, sizeof(handler_file));

    std::string buffer;
    name_render_.GetCopyright("phxrpc_pb2service", stree->GetProtoFile(), &buffer, false);

    fprintf(write, "/* %s.cpp\n", filename);
    fprintf(write, "%s", buffer.c_str());
    fprintf(write, "*/\n");
    fprintf(write, "\n");

    fprintf(write, "#include \"%s.h\"\n", filename);
    fprintf(write, "#include \"%s.h\"\n", config_file);
    fprintf(write, "#include \"%s.h\"\n", handler_file);

    name_render_.GetMessageFileName(stree->GetProtoFile(), filename, sizeof(filename));
    fprintf(write, "#include \"%s.h\"\n", filename);
    fprintf(write, "#include \"phxrpc/file.h\"\n");

    fprintf(write, "\n");

    char clasname[128] = { 0 }, config_name[128] = { 0 };
    name_render_.GetServiceImplClasname(stree->GetName(), clasname, sizeof(clasname));
    name_render_.GetServerConfigClasname(stree->GetName(), config_name, sizeof(config_name));

    fprintf(write, "%s :: %s( ServiceArgs_t & args )\n", clasname, clasname);
    fprintf(write, "    : args_( args ), svr_handler_( args.svr_handler_ )\n");
    fprintf(write, "{\n");
    fprintf(write, "}\n");
    fprintf(write, "\n");

    fprintf(write, "%s :: ~%s()\n", clasname, clasname);
    fprintf(write, "{\n");
    fprintf(write, "}\n");
    fprintf(write, "\n");

    SyntaxFuncVector * flist = stree->GetFuncList();
    SyntaxFuncVector::iterator fit = flist->begin();
    for (; flist->end() != fit; ++fit) {
        std::string buffer;
        GetServiceFuncDeclaration(stree, &(*fit), 0, 1, 1, &buffer);
        fprintf(write, "%s\n", buffer.c_str());
        fprintf(write, "{\n");

        if (0 == strcmp("PHXEcho", fit->GetName())) {
            fprintf(write, "    resp->set_value( req.value() );\n");
            fprintf(write, "    return 0;\n");
        } else {
            //fprintf(write, "    return -1;\n");
            std::string handler_buffer;
            GetServiceFuncInvokeHandler(stree, &(*fit), &handler_buffer);
            fprintf(write, "    return %s;\n", handler_buffer.c_str());
        }
        fprintf(write, "}\n");
        fprintf(write, "\n");
    }

    fprintf(write, "\n");
}

void ServiceCodeRender::GetServiceFuncDeclaration(SyntaxTree * stree, SyntaxFunc * func, int is_header, int is_impl,
                                                  int need_param_name, std::string * result) {
    char clasname[128] = { 0 }, type_name[128] = { 0 };

    if (is_impl) {
        name_render_.GetServiceImplClasname(stree->GetName(), clasname, sizeof(clasname));
    } else {
        name_render_.GetServiceClasname(stree->GetName(), clasname, sizeof(clasname));
    }

    if (is_header) {
        StrAppendFormat(result, "int %s( ", func->GetName());
    } else {
        StrAppendFormat(result, "int %s :: %s( ", clasname, func->GetName());
    }

    name_render_.GetMessageClasname(func->GetReq()->GetType(), type_name, sizeof(type_name));
    StrAppendFormat(result, "const %s & %s,\n", type_name, need_param_name ? "req" : "/* req */");

    name_render_.GetMessageClasname(func->GetResp()->GetType(), type_name, sizeof(type_name));
    StrAppendFormat(result, "        %s * %s", type_name, need_param_name ? "resp" : "/* resp */");

    result->append(" )");
}

void ServiceCodeRender::GetServiceFuncInvokeHandler(SyntaxTree * stree, SyntaxFunc * func, std::string * result) {
    char type_name[128] = { 0 }, cpp_type[128] = { 0 };

    StrAppendFormat(result, "svr_handler_->%s( ", func->GetName());

    name_render_.GetMessageClasname(func->GetReq()->GetType(), type_name, sizeof(type_name));
    name_render_.GetFuncCppDeclaration(type_name, cpp_type, sizeof(cpp_type));

    int has_req = 1;
    if (0 == strlen(cpp_type)) {
        has_req = 0;
    } else {
        StrAppendFormat(result, "%s", (0 == strcmp(cpp_type, type_name)) ? "req" : "req.value()");
    }

    name_render_.GetMessageClasname(func->GetResp()->GetType(), type_name, sizeof(type_name));
    name_render_.GetFuncCppDeclaration(type_name, cpp_type, sizeof(cpp_type));

    if (0 != strlen(cpp_type)) {
        StrAppendFormat(result, "%s", has_req ? "," : "");
        StrAppendFormat(result, "%s", (0 == strcmp(cpp_type, type_name)) ? "resp" : "resp->mutable_value()");
    }

    result->append(" )");
}


void ServiceCodeRender::GenerateServerHandlerHpp(SyntaxTree * stree, FILE * write) {
    char filename[128] = { 0 };
    name_render_.GetServerHandlerFileName(stree->GetName(), filename, sizeof(filename));

    std::string buffer;
    name_render_.GetCopyright("phxrpc_pb2service", stree->GetProtoFile(), &buffer, false);

    fprintf(write, "/* %s.h\n", filename);
    fprintf(write, "%s", buffer.c_str());
    fprintf(write, "*/\n");
    fprintf(write, "\n");

    fprintf(write, "#pragma once\n");

    fprintf(write, "\n");

    name_render_.GetMessageFileName(stree->GetProtoFile(), filename, sizeof(filename));
    fprintf(write, "#include \"%s.h\"\n", filename);

    char handler_name[128] = { 0 };
    name_render_.GetServerHandlerClasname(stree->GetName(), handler_name, sizeof(handler_name));
    // name_render_.GetServerConfigClasname(stree->GetName(), config_name, sizeof(config_name));
    fprintf(write, "\n");
    fprintf(write, "class %s\n", handler_name);
    fprintf(write, "{\n");

    fprintf(write, "public:\n");
    // fprintf(write, "    %s( ServiceArgs_t * app_args );\n", clasname);
    fprintf(write, "    %s( );\n", handler_name);
    fprintf(write, "    virtual ~%s();\n", handler_name);
    fprintf(write, "\n");

    fprintf(write, "public:\n");
    fprintf(write, "    void BeforeRun();\n");
    fprintf(write, "    void AfterRun();\n");
    fprintf(write, "\n");

    fprintf(write, "public:\n");
    SyntaxFuncVector * flist = stree->GetFuncList();
    SyntaxFuncVector::iterator fit = flist->begin();
    for (; flist->end() != fit; ++fit) {
        if (0 == strcmp("PHXEcho", fit->GetName())) {
            continue;
        }

        std::string buffer;

        GetServerHandlerFuncDeclaration(stree, &(*fit), 1, 1, &buffer);
        fprintf(write, "    virtual %s;\n", buffer.c_str());
        fprintf(write, "\n");
    }

    // fprintf(write, "private:\n");
    // fprintf(write, "    const %s & config_;\n", config_name);

    fprintf(write, "};\n");

    fprintf(write, "\n");
}


void ServiceCodeRender::GenerateServerHandlerCpp(SyntaxTree * stree, FILE * write) {
    char handler_file[128] = { 0 };
    name_render_.GetServerHandlerFileName(stree->GetName(), handler_file, sizeof(handler_file));

    std::string buffer;
    name_render_.GetCopyright("phxrpc_pb2service", stree->GetProtoFile(), &buffer, false);

    fprintf(write, "/* %s.cpp\n", handler_file);
    fprintf(write, "%s", buffer.c_str());
    fprintf(write, "*/\n");
    fprintf(write, "\n");

    fprintf(write, "#include \"%s.h\"\n", handler_file);
    fprintf(write, "\n");

    char handler_name[128] = { 0 };
    name_render_.GetServerHandlerClasname(stree->GetName(), handler_name, sizeof(handler_name));

    fprintf(write, "%s :: %s( )\n", handler_name, handler_name);
    fprintf(write, "{\n");
    fprintf(write, "}\n");
    fprintf(write, "\n");

    fprintf(write, "%s :: ~%s()\n", handler_name, handler_name);
    fprintf(write, "{\n");
    fprintf(write, "}\n");
    fprintf(write, "\n");

    fprintf(write, "void %s :: %s( )\n", handler_name, "BeforeRun");
    fprintf(write, "{\n");
    fprintf(write, "}\n");
    fprintf(write, "\n");

    fprintf(write, "void  %s :: %s( )\n", handler_name, "AfterRun");
    fprintf(write, "{\n");
    fprintf(write, "}\n");
    fprintf(write, "\n");

    SyntaxFuncVector * flist = stree->GetFuncList();
    SyntaxFuncVector::iterator fit = flist->begin();
    for (; flist->end() != fit; ++fit) {
        if (0 == strcmp("PHXEcho", fit->GetName())) {
            continue;
        }
        std::string buffer;
        GetServerHandlerFuncDeclaration(stree, &(*fit), 0, 1, &buffer);
        fprintf(write, "%s\n", buffer.c_str());
        fprintf(write, "{\n");

        fprintf(write, "    return -1;\n");
        fprintf(write, "}\n");
        fprintf(write, "\n");
    }

    fprintf(write, "\n");
}


void ServiceCodeRender::GetServerHandlerFuncDeclaration(SyntaxTree *stree, SyntaxFunc *func, int is_header,
                                                        int need_param_name, std::string *result) {
    char clasname[128] = { 0 }, type_name[128] = { 0 }, cpp_type[128] = { 0 };

    name_render_.GetServerHandlerClasname(stree->GetName(), clasname, sizeof(clasname));

    if (is_header) {
        StrAppendFormat(result, "int %s( ", func->GetName());
    } else {
        StrAppendFormat(result, "int %s :: %s( ", clasname, func->GetName());
    }

    name_render_.GetMessageClasname(func->GetReq()->GetType(), type_name, sizeof(type_name));
    name_render_.GetFuncCppDeclaration(type_name, cpp_type, sizeof(cpp_type));

    int has_req = 1;
    if ( 0 == strlen(cpp_type)) {
        has_req = 0;
    } else {
        StrAppendFormat(result, "const %s & %s", cpp_type, need_param_name ? "req" : "/* req */");
    }

    name_render_.GetMessageClasname(func->GetResp()->GetType(), type_name, sizeof(type_name));
    name_render_.GetFuncCppDeclaration(type_name, cpp_type, sizeof(cpp_type));

    if ( 0 != strlen(cpp_type)) {
        StrAppendFormat(result, "%s", has_req ? ",\n" : "");
        StrAppendFormat(result, "%s", has_req ? "        " : "");

        StrAppendFormat(result, "%s * %s", cpp_type, need_param_name ? "resp" : "/* resp */");
    }

    result->append(" )");

}


void ServiceCodeRender::GenerateDispatcherHpp(SyntaxTree * stree, FILE * write) {
    char filename[128] = { 0 };
    name_render_.GetDispatcherFileName(stree->GetName(), filename, sizeof(filename));

    std::string buffer;
    name_render_.GetCopyright("phxrpc_pb2service", stree->GetProtoFile(), &buffer);

    fprintf(write, "/* %s.h\n", filename);
    fprintf(write, "%s", buffer.c_str());
    fprintf(write, "*/\n");
    fprintf(write, "\n");

    fprintf(write, "#pragma once\n");

    fprintf(write, "\n");

    fprintf(write, "#include \"phxrpc/http.h\"\n");
    fprintf(write, "#include \"phxrpc/rpc.h\"\n");

    fprintf(write, "\n");

    char clasname[128] = { 0 }, service_name[128] = { 0 };
    name_render_.GetDispatcherClasname(stree->GetName(), clasname, sizeof(clasname));
    name_render_.GetServiceClasname(stree->GetName(), service_name, sizeof(service_name));

    fprintf(write, "class %s;\n", service_name);
    fprintf(write, "\n");

    fprintf(write, "class %s\n", clasname);
    fprintf(write, "{\n");
    fprintf(write, "public:\n");

    fprintf(write, "    %s( %s & service, phxrpc::DispatcherArgs_t * dispatcher_args );\n", clasname, service_name);
    fprintf(write, "\n");

    fprintf(write, "    ~%s();\n", clasname);
    fprintf(write, "\n");

    SyntaxFuncVector * flist = stree->GetFuncList();
    SyntaxFuncVector::iterator fit = flist->begin();
    for (; flist->end() != fit; ++fit) {
        fprintf(write, "    int %s( const phxrpc::HttpRequest & request, "
                "phxrpc::HttpResponse * response );\n",
                fit->GetName());
        fprintf(write, "\n");
    }

    fprintf(write, "private:\n");
    fprintf(write, "    %s & service_;\n", service_name);
    fprintf(write, "    phxrpc::DispatcherArgs_t * dispatcher_args_;\n" );
    fprintf(write, "\n");

    fprintf(write, "public:\n");
    fprintf(write, "    static const phxrpc::HttpDispatcher< %s >::URIFuncMap & GetURIFuncMap();\n", clasname);
    fprintf(write, "\n");

    fprintf(write, "};\n");

    fprintf(write, "\n");
}

void ServiceCodeRender::GenerateDispatcherCpp(SyntaxTree * stree, FILE * write) {
    char filename[128] = { 0 };
    name_render_.GetDispatcherFileName(stree->GetName(), filename, sizeof(filename));

    std::string buffer;
    name_render_.GetCopyright("phxrpc_pb2service", stree->GetProtoFile(), &buffer);

    fprintf(write, "/* %s.h\n", filename);
    fprintf(write, "%s", buffer.c_str());
    fprintf(write, "*/\n");
    fprintf(write, "\n");

    fprintf(write, "#include \"%s.h\"\n", filename);
    fprintf(write, "\n");

    name_render_.GetMessageFileName(stree->GetProtoFile(), filename, sizeof(filename));
    fprintf(write, "#include \"%s.h\"\n", filename);

    name_render_.GetServiceFileName(stree->GetName(), filename, sizeof(filename));
    fprintf(write, "#include \"%s.h\"\n", filename);

    fprintf(write, "#include \"phxrpc/http.h\"\n");
    fprintf(write, "#include \"phxrpc/file.h\"\n");
    fprintf(write, "#include <errno.h>\n");

    fprintf(write, "\n");

    char clasname[128] = { 0 }, service_name[128] = { 0 };
    name_render_.GetDispatcherClasname(stree->GetName(), clasname, sizeof(clasname));
    name_render_.GetServiceClasname(stree->GetName(), service_name, sizeof(service_name));

    fprintf(write, "%s :: %s( %s & service, phxrpc::DispatcherArgs_t * dispatcher_args )\n", clasname, clasname, service_name);
    fprintf(write, "    : service_( service ), dispatcher_args_(dispatcher_args)\n");

    fprintf(write, "{\n");
    fprintf(write, "}\n");
    fprintf(write, "\n");

    fprintf(write, "%s :: ~%s()\n", clasname, clasname);
    fprintf(write, "{\n");
    fprintf(write, "}\n");
    fprintf(write, "\n");

    GenerateURIFuncMap(stree, write);

    fprintf(write, "\n");

    SyntaxFuncVector * flist = stree->GetFuncList();
    SyntaxFuncVector::iterator fit = flist->begin();
    for (; flist->end() != fit; ++fit) {
        GenerateDispatcherFunc(stree, &(*fit), write);
    }

    fprintf(write, "\n");
}

void ServiceCodeRender::GenerateURIFuncMap(SyntaxTree * stree, FILE * write) {
    char clasname[128] = { 0 };
    name_render_.GetDispatcherClasname(stree->GetName(), clasname, sizeof(clasname));

    fprintf(write, "const phxrpc::HttpDispatcher< %s >::URIFuncMap & %s :: GetURIFuncMap()\n", clasname, clasname);
    fprintf(write, "{\n");

    fprintf(write, "    static phxrpc::HttpDispatcher< %s >::URIFuncMap uri_func_map = {\n", clasname);

    SyntaxFuncVector * flist = stree->GetFuncList();
    SyntaxFuncVector::iterator fit = flist->begin();
    for (; flist->end() != fit; ++fit) {
        if (fit != flist->begin()) {
            fprintf(write, ",\n");
        }
        fprintf(write, "        {\"/%s/%s\", &%s::%s}", stree->GetPackageName(), fit->GetName(), clasname,
                fit->GetName());
    }
    fprintf(write, "};\n");

    fprintf(write, "    return uri_func_map;\n");

    fprintf(write, "}\n");
}

void ServiceCodeRender::GenerateDispatcherFunc(SyntaxTree * stree, SyntaxFunc * func, FILE * write) {
    char clasname[128] = { 0 }, type_name[128] = { 0 };

    name_render_.GetDispatcherClasname(stree->GetName(), clasname, sizeof(clasname));

    fprintf(write, "int %s :: %s( const phxrpc::HttpRequest & request, "
            "phxrpc::HttpResponse * response )\n",
            clasname, func->GetName());

    fprintf(write, "{\n");
    fprintf(write, "    dispatcher_args_->server_monitor->SvrCall(%d, \"%s\", 1);\n", func->GetCmdID(), func->GetName() );
    fprintf(write, "\n");

    fprintf(write, "    int ret = 0;\n");
    fprintf(write, "\n");

    name_render_.GetMessageClasname(func->GetReq()->GetType(), type_name, sizeof(type_name));
    fprintf(write, "    %s req;\n", type_name);

    name_render_.GetMessageClasname(func->GetResp()->GetType(), type_name, sizeof(type_name));
    fprintf(write, "    %s resp;\n", type_name);

    fprintf(write, "\n");

    fprintf(write, "    //unpack request\n");
    fprintf(write, "    {\n");

    fprintf(write, "        if( ! req.ParseFromString( request.GetContent() ) )\n");
    fprintf(write, "        {\n");

    fprintf(write, "            phxrpc::log( LOG_ERR, \"ERROR: FromBuffer fail size %%zu ip %%s\",\n"
            "                request.GetContent().size(), request.GetClientIP() );\n");

    fprintf(write, "            return -1 * EINVAL;\n");
    fprintf(write, "        }\n");
    fprintf(write, "    }\n");

    fprintf(write, "\n");

    fprintf(write, "    //logic process\n");
    fprintf(write, "    {\n");

    fprintf(write, "        if( 0 == ret ) ret = service_.%s( req, &resp );\n", func->GetName());

    fprintf(write, "    }\n\n");

    fprintf(write, "    //pack response\n");
    fprintf(write, "    {\n");
    fprintf(write, "        if( ! resp.SerializeToString( &( response->GetContent() ) ) )\n");
    fprintf(write, "        {\n");

    fprintf(write, "            phxrpc::log( LOG_ERR, \"ERROR: ToBuffer fail ip %%s\", request.GetClientIP() );\n");

    fprintf(write, "            return -1 * ENOMEM;\n");
    fprintf(write, "        }\n");
    fprintf(write, "    }\n");

    fprintf(write, "\n");

    fprintf(write, "    phxrpc::log( LOG_DEBUG, \"RETN: %s = %%d\", ret );\n", func->GetName());

    fprintf(write, "\n");
    fprintf(write, "    return ret;\n");
    fprintf(write, "}\n");
    fprintf(write, "\n");
}

