/* phxbinlog_service_impl.h

 Generated by phxrpc_pb2service from phxbinlogsvr.proto

*/

#pragma once

#include "phxbinlogsvr.pb.h"
#include "phxrpc_phxbinlog_service.h"
#include "phxbinlog_server_handler.h"

class PhxbinlogServerConfig;
class PhxbinlogServerHandler;

typedef struct tagServiceArgs {
    PhxbinlogServerConfig * config;
    PhxbinlogServerHandler * svr_handler_;
    //You can add other arguments here and initiate in main().
}ServiceArgs_t;

class PhxbinlogServiceImpl : public PhxbinlogService
{
public:
    PhxbinlogServiceImpl( ServiceArgs_t & args );
    virtual ~PhxbinlogServiceImpl();

    virtual int PHXEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp );

    virtual int SendBinLog( const google::protobuf::BytesValue & req,
        google::protobuf::Empty * resp );

    virtual int GetMasterInfoFromGlobal( const google::protobuf::Empty & req,
        google::protobuf::BytesValue * resp );

    virtual int GetMasterInfoFromLocal( const google::protobuf::Empty & req,
        google::protobuf::BytesValue * resp );

    virtual int GetLastSendGtidFromGlobal( const google::protobuf::BytesValue & req,
        google::protobuf::BytesValue * resp );

    virtual int GetLastSendGtidFromLocal( const google::protobuf::BytesValue & req,
        google::protobuf::BytesValue * resp );

    virtual int SetExportIP( const google::protobuf::BytesValue & req,
        google::protobuf::Empty * resp );

    virtual int HeartBeat( const google::protobuf::BytesValue & req,
        google::protobuf::Empty * resp );

    virtual int AddMember( const google::protobuf::BytesValue & req,
        google::protobuf::Empty * resp );

    virtual int RemoveMember( const google::protobuf::BytesValue & req,
        google::protobuf::Empty * resp );

    virtual int SetMySqlAdminInfo( const google::protobuf::BytesValue & req,
        google::protobuf::Empty * resp );

    virtual int SetMySqlReplicaInfo( const google::protobuf::BytesValue & req,
        google::protobuf::Empty * resp );

    virtual int GetMemberList( const google::protobuf::Empty & req,
        google::protobuf::BytesValue * resp );

    virtual int InitBinlogSvrMaster( const google::protobuf::BytesValue & req,
        google::protobuf::Empty * resp );

private:
    ServiceArgs_t & args_;
    PhxbinlogServerHandler * svr_handler_;
};
