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
//////////////////////////////////////////////////////////////////////

const char * SVRKIT_CLIENT_MAKEFILE_DEFINE_TEMPLATE =
R"(
vpath %.proto $(SRC_BASE_PATH)/$ProtoPath$
$StubFile$.cpp: $ProtoFile$
	$(PHXRPC_INCLUDE_PATH)/codegen/phxrpc_pb2client -f $ProtoFile$ -I $(SRC_BASE_PATH)/$ProtoPath$/ -I $(PROTOBUF_INCLUDE_PATH) -d .
)";
//$StubFile$.h: $ProtoFile$
//$(PHXRPC_ROOT)/codegen/phxrpc_pb2client $(PBFLAGS) -f $^ -d .

const char * SVRKIT_SERVER_MAKEFILE_DEFINE_TEMPLATE =
R"(
vpath %.proto $(SRC_BASE_PATH)/$ProtoPath$
$DispatcherFile$.cpp: $ProtoFile$
	$(PHXRPC_INCLUDE_PATH)/codegen/phxrpc_pb2service -f $ProtoFile$ -I $(SRC_BASE_PATH)/$ProtoPath$/ -I $(PROTOBUF_INCLUDE_PATH) -d .
$ServiceFile$.cpp: $ProtoFile$
	$(PHXRPC_INCLUDE_PATH)/codegen/phxrpc_pb2service -f $ProtoFile$ -I $(SRC_BASE_PATH)/$ProtoPath$/ -I $(PROTOBUF_INCLUDE_PATH) -d .
)";

//$ServiceFile$.h: $ProtoFile$
//$(PHXRPC_ROOT)/codegen/phxrpc_pb2service $(PBFLAGS) -f $^ -d .

const char * SVRKIT_TOOLS_MAKEFILE_DEFINE_TEMPLATE =
R"(
vpath %.proto $(SRC_BASE_PATH)/$ProtoPath$
$ToolFile$.cpp: $ProtoFile$
	$(PHXRPC_INCLUDE_PATH)/codegen/phxrpc_pb2tool -f $ProtoFile$ -I $(SRC_BASE_PATH)/$ProtoPath$/ -I $(PROTOBUF_INCLUDE_PATH) -d .
)";

/*
const char * PHXRPC_SERVER_MAKEFILE_TEMPLATE =
R"(

include $PhxRPCMKDir$/phxrpc.mk

LDFLAGS := -L$(PHXRPC_ROOT)/lib -lphxrpc $(LDFLAGS)

#choose to use boost for network
#LDFLAGS := $(PLUGIN_BOOST_LDFLAGS) $(LDFLAGS)

SVR_OBJS = $MessageFile$.o \
           $ServerHandlerFile$.o \
		   $ServiceImplFile$.o \
		   $ServiceFile$.o \
		   $DispatcherFile$.o \
		   $ServerConfigFile$.o \
		   $ServerMainFile$.o

CLI_OBJS = $MessageFile$.o \
		   $ClientFile$.o \
		   $StubFile$.o

TARGETS = lib$ClientFile$.a $ServerMainFile$ $ToolMainFile$

all: $(TARGETS)

$ServerMainFile$: $(SVR_OBJS)
	$(LINKER) $^ $(LDFLAGS) -o $@

lib$ClientFile$.a: $(CLI_OBJS)
	$(AR) $@ $^

$ToolMainFile$: $ToolFile$.o $ToolImplFile$.o $ToolMainFile$.o
	$(LINKER) $^ -L. -l$ClientFile$ $(LDFLAGS) -o $@

########## message ##########

$MessageFile$.cc: $MessageFile$.h

$MessageFile$.h: $ProtoFile$
	$(PROTOBUF_ROOT)/bin/protoc -I$(PROTOBUF_ROOT)/include --cpp_out=. -I$(PHXRPC_ROOT) -I. $^

########## client ##########

$StubFile$.cpp: $StubFile$.h
$StubFile$.o: $StubFile$.h
$ClientFile$.cpp: $StubFile$.h
$ClientFile$.o: $StubFile$.h

$StubFile$.h: $ProtoFile$
	$(PHXRPC_ROOT)/codegen/phxrpc_pb2client $(PBFLAGS) -f $^ -d .

########## service ##########

$ServiceFile$.cpp: $ServiceFile$.h
$ServiceFile$.o: $ServiceFile$.h
$ServiceImplFile$.cpp: $ServiceFile$.h
$ServiceImplFile$.o: $ServiceFile$.h
$DispatcherFile$.cpp: $ServiceFile$.h
$DispatcherFile$.o: $ServiceFile$.h

$ServerHandlerFile$.cpp : $ServiceFile$.h
$ServerHandlerFile$.o : $ServiceFile$.h

$ServiceFile$.h: $ProtoFile$
	$(PHXRPC_ROOT)/codegen/phxrpc_pb2service $(PBFLAGS) -f $^ -d .

########## tool ##########

$ToolFile$.cpp: $ToolFile$.h
$ToolFile$.o: $ToolFile$.h
$ToolImplFile$.cpp: $ToolFile$.h
$ToolImplFile$.o: $ToolFile$.h
$ToolMainFile$.cpp: $ToolFile$.h
$ToolMainFile$.o: $ToolFile$.h

$ToolFile$.h: $ProtoFile$
	$(PHXRPC_ROOT)/codegen/phxrpc_pb2tool $(PBFLAGS) -f $^ -d .

clean:
	@($(RM) $(TARGETS))
	@($(RM) *.o)
	@($(RM) phxrpc_*)
	@($(RM) *.pb.*)

)";
*/
