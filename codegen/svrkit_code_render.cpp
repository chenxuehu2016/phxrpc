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

#include "svrkit_code_render.h"
#include "svrkit_template.h"

#include "name_render.h"
#include "syntax_tree.h"
#include "code_utils.h"

#include <cstring>
using namespace phxrpc;
using namespace std;

SvrKitCodeRender::SvrKitCodeRender(NameRender & name_render)
        : name_render_(name_render) {
}

SvrKitCodeRender::~SvrKitCodeRender() {
}

string SvrKitCodeRender::GetPhxrpcClientVpath(const char* pbname, const std::string & pbpath, const std::string & pbfile) {
	std::string buffer;
	char stub_file[128] = { 0 };

	name_render_.GetStubFileName(pbname, stub_file, sizeof(stub_file));

	std::string content(SVRKIT_CLIENT_MAKEFILE_DEFINE_TEMPLATE);
	StrTrim(&content);
	StrReplaceAll(&content, "$StubFile$", stub_file);
	StrReplaceAll(&content, "$ProtoPath$", pbpath);
	StrReplaceAll(&content, "$ProtoFile$", pbfile);

	return content;
}

string SvrKitCodeRender::GetPhxrpcServerVpath(const char* pbname, const std::string & pbpath, const std::string & pbfile) {
	std::string buffer;
	char dispatcher_file[128] = { 0 }, service_file[128] = { 0 };

	name_render_.GetDispatcherFileName(pbname, dispatcher_file, sizeof(dispatcher_file));
	name_render_.GetServiceFileName(pbname, service_file, sizeof(service_file));

	std::string content(SVRKIT_SERVER_MAKEFILE_DEFINE_TEMPLATE);
	StrTrim(&content);
	StrReplaceAll(&content, "$DispatcherFile$", dispatcher_file);
	StrReplaceAll(&content, "$ServiceFile$", service_file);
	StrReplaceAll(&content, "$ProtoPath$", pbpath);
	StrReplaceAll(&content, "$ProtoFile$", pbfile);

	return content;
}

string SvrKitCodeRender::GetPhxrpcToolsVpath(const char* pbname, const std::string & pbpath, const std::string & pbfile) {
	std::string buffer;
	char tool_file[128] = { 0 };

	name_render_.GetToolFileName(pbname, tool_file, sizeof(tool_file));

	std::string content(SVRKIT_TOOLS_MAKEFILE_DEFINE_TEMPLATE);
	StrTrim(&content);
	StrReplaceAll(&content, "$ToolFile$", tool_file);
	StrReplaceAll(&content, "$ProtoPath$", pbpath);
	StrReplaceAll(&content, "$ProtoFile$", pbfile);

	return content;
}

std::string SvrKitCodeRender::GetPhxrpcClientObjs(const char *pbname, const std::string &pbpath, const std::string &pbfile) {
	//	string obj_str;
//	if (strcasecmp(phxrpc, "client") == 0) {
//		obj_str.append("phxrpc_phxbinlog_stub.o phxbinlog_stub_phxrpc.o phxbinlog_client_factory_phxrpc.o");
	std::string buffer;

	char stub_file[128] = { 0 }, client_file[128] = { 0 };

	name_render_.GetStubFileName(pbname, stub_file, sizeof(stub_file));
	name_render_.GetToolFileName(pbname, client_file, sizeof(client_file));

	std::string content = "$StubFile$.o $ClientFile$.o";

	StrReplaceAll(&content, "$StubFile$", stub_file);
	StrReplaceAll(&content, "$ClientFile$", client_file);

	return content;
}

std::string SvrKitCodeRender::GetPhxrpcServerObjs(const char *pbname, const std::string &pbpath, const std::string &pbfile) {
//	} else if (strcasecmp(phxrpc, "svr") == 0) {
//		obj_str.append("phxrpc_phxbinlog_service.o phxrpc_phxbinlog_dispatcher.o phxbinlog_server_config.o phxbinlog_service_impl.o phxbinlog_server.o phxbinlog_main.o");

	std::string buffer;

	char dispatcher_file[128] = { 0 }, service_file[128] = { 0 }, service_impl_file[128] = { 0 }, server_handler_file[128] = { 0 };
	char server_config_file[128] = { 0 }, server_main_file[128] = { 0 };

	name_render_.GetDispatcherFileName(pbname, dispatcher_file, sizeof(dispatcher_file));
	name_render_.GetServiceImplFileName(pbname, service_impl_file, sizeof(service_impl_file));
	name_render_.GetServiceFileName(pbname, service_file, sizeof(service_file));
	name_render_.GetServerHandlerFileName(pbname, server_handler_file, sizeof(server_handler_file));

	name_render_.GetServerConfigFileName(pbname, server_config_file, sizeof(server_config_file));
	name_render_.GetServerMainFileName(pbname, server_main_file, sizeof(server_main_file));

	std::string content = R"($ServerHandlerFile$.o  $ServiceImplFile$.o $ServiceFile$.o $DispatcherFile$.o $ServerConfigFile$.o $ServerMainFile$.o)";

	StrReplaceAll(&content, "$DispatcherFile$", dispatcher_file);
	StrReplaceAll(&content, "$ServiceFile$", service_file);
	StrReplaceAll(&content, "$ServiceImplFile$", service_impl_file);
	StrReplaceAll(&content, "$ServerHandlerFile$", server_handler_file);


	StrReplaceAll(&content, "$ServerConfigFile$", server_config_file);
	StrReplaceAll(&content, "$ServerMainFile$", server_main_file);

	return content;
}

std::string SvrKitCodeRender::GetPhxrpcToolsObjs(const char *pbname, const std::string &pbpath, const std::string &pbfile) {
	//	} else if (strcasecmp(phxrpc, "tools") == 0) {
//		obj_str.append("phxrpc_phxbinlog_tool.o phxbinlog_tool_impl.o phxbinlog_tool_main.o");
	std::string buffer;

	char tool_file[128] = { 0 }, tool_impl_file[128] = { 0 }, tool_main_file[128] = { 0 };

	name_render_.GetToolImplFileName(pbname, tool_impl_file, sizeof(tool_impl_file));
	name_render_.GetToolFileName(pbname, tool_file, sizeof(tool_file));
	name_render_.GetToolMainFileName(pbname, tool_main_file, sizeof(tool_main_file));

	std::string content = "$ToolFile$.o $ToolImplFile$.o $ToolMainFile$.o";

	StrReplaceAll(&content, "$ToolFile$", tool_file);
	StrReplaceAll(&content, "$ToolImplFile$", tool_impl_file);
	StrReplaceAll(&content, "$ToolMainFile$", tool_main_file);

	return content;
}

std::string SvrKitCodeRender::GetPhxrpcMessageObjs(const char *pbname, const std::string &pbpath, const std::string &pbfile) {
	std::string content(pbfile);

	StrReplaceAll(&content, ".proto", ".pb.o");

	return content;
}


/*void GenerateMakefile1(SyntaxTree * stree, const std::string & mk_dir_path, FILE * write) {
    std::string buffer;
    name_render_.GetCopyright("phxrpc_pb2server", stree->GetProtoFile(), &buffer, false, "#");

    fprintf(write, "# Makefile\n");
    fprintf(write, "%s", buffer.c_str());
    fprintf(write, "#\n");
    fprintf(write, "\n");

    char dispatcher_file[128] = { 0 }, service_file[128] = { 0 }, service_impl_file[128] = { 0 }, server_handler_file[128] = { 0 };
    char server_config_file[128] = { 0 }, server_main_file[128] = { 0 };
    char message_file[128] = { 0 }, stub_file[128] = { 0 }, client_file[128] = { 0 };
    char tool_file[128] = { 0 }, tool_impl_file[128] = { 0 }, tool_main_file[128] = { 0 };

    name_render_.GetDispatcherFileName(stree->GetName(), dispatcher_file, sizeof(dispatcher_file));
    name_render_.GetServiceImplFileName(stree->GetName(), service_impl_file, sizeof(service_impl_file));
    name_render_.GetServiceFileName(stree->GetName(), service_file, sizeof(service_file));
    name_render_.GetServerHandlerFileName(stree->GetName(), server_handler_file, sizeof(server_handler_file));

    name_render_.GetServerConfigFileName(stree->GetName(), server_config_file, sizeof(server_config_file));
    name_render_.GetServerMainFileName(stree->GetName(), server_main_file, sizeof(server_main_file));

    name_render_.GetToolImplFileName(stree->GetName(), tool_impl_file, sizeof(tool_impl_file));
    name_render_.GetToolFileName(stree->GetName(), tool_file, sizeof(tool_file));
    name_render_.GetToolMainFileName(stree->GetName(), tool_main_file, sizeof(tool_main_file));

    name_render_.GetClientFileName(stree->GetName(), client_file, sizeof(client_file));
    name_render_.GetStubFileName(stree->GetName(), stub_file, sizeof(stub_file));
    name_render_.GetMessageFileName(stree->GetProtoFile(), message_file, sizeof(message_file));

    std::string content(PHXRPC_SERVER_MAKEFILE_TEMPLATE);
    StrTrim(&content);
    StrReplaceAll(&content, "$PhxRPCMKDir$", mk_dir_path);
    StrReplaceAll(&content, "$DispatcherFile$", dispatcher_file);
    StrReplaceAll(&content, "$ServiceFile$", service_file);
    StrReplaceAll(&content, "$ServiceImplFile$", service_impl_file);
    StrReplaceAll(&content, "$ServerHandlerFile$", server_handler_file);


    StrReplaceAll(&content, "$ServerConfigFile$", server_config_file);
    StrReplaceAll(&content, "$ServerMainFile$", server_main_file);

    StrReplaceAll(&content, "$MessageFile$", message_file);
    StrReplaceAll(&content, "$StubFile$", stub_file);
    StrReplaceAll(&content, "$ClientFile$", client_file);

    StrReplaceAll(&content, "$ToolFile$", tool_file);
    StrReplaceAll(&content, "$ToolImplFile$", tool_impl_file);
    StrReplaceAll(&content, "$ToolMainFile$", tool_main_file);

    StrReplaceAll(&content, "$ProtoFile$", stree->GetProtoFile());

    fprintf(write, "%s", content.c_str());

    fprintf(write, "\n\n");
}*/

