<img align="right" src="http://mmbiz.qpic.cn/mmbiz/UqFrHRLeCAkOcYOjaX3oxIxWicXVJY0ODsbAyPybxk4DkPAaibgdm7trm1MNiatqJYRpF034J7PlfwCz33mbNUkew/640?wx_fmt=jpeg&wxfrom=5&wx_lazy=1" hspace="15" width="150px" style="float: right">

**PhxRPC是微信后台团队推出的一个非常简洁小巧的RPC框架，编译生成的库只有450K。**

# 更多文档请[官方文档(https://github.com/tencent-wechat/phxrpc/blob/master/README.md)

# 为什么要扩展phxrpc?
根据[微信技术总监周颢：一亿用户背后架构秘密](http://tech.qq.com/a/20120515/000224.htm)

`
因为网络协议设计本身有非常多的功能也会有比较大的字段，相关的代码可能会有数千行，这一块不能通过手写方式完成。可以通过XML描述，再通过工具自动生成所有的代码，这是微信获得快速开发的一个重要的点。
`

`
Svrkit —— Client/Server自动代码生成框架:10分钟搭建内部服务器
随时添加新逻辑OssAgent——监控/统计框架：所见即所得的监控报表存储组件——屏蔽容灾/扩容等复杂问题
`

本项目的目的就是扩展腾讯开源[phxrpc](https://github.com/tencent-wechat/phxrpc)实现开源的 `Svrkit`

# 如何使用
#### 编写xml文件
下面是sample目录下的xml文件样例。

```xml
<?xml version="1.0"?>
<!-- sudo apt install libxml2-utils; xmllint &#45;&#45;noout &#45;&#45;schema ../schema.xsd phxbinlogsvr.xml -->
<phxrpc xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
        xsi:noNamespaceSchemaLocation="../schema.xsd">
    <package>phxbinlogsvr</package>
    <message name="BinLogBuffer">
        <field type="string" name="oldgtid">1</field>
        <field type="string" name="newgtid">2</field>
        <field type="bytes" name="eventbuffer">3</field>
    </message>
    <message name="BinLogResp">
        <field type="int32" repeated="true" name="section">1</field>
        <field type="bytes" name="respbuffer">2</field>
    </message>
    <imports>
        <import>google/protobuf/wrappers.proto</import>
        <import>google/protobuf/empty.proto</import>
        <import>phxrpc/rpc/phxrpc.proto</import>
    </imports>
    <service name="phxbinlog">
        <rpc name="SendBinLog" paramter="google.protobuf.BytesValue" returns="google.protobuf.Empty">
            <cmdID>1</cmdID>
            <optString>e:</optString>
            <usage>-e &lt;emtpy&gt;</usage>
        </rpc>
    </service>
</phxrpc>
```

使用 xmllint校验xml文件正确性
```bash
xmllint --noout --schema ../schema.xsd phxbinlogsvr.xml
```

### 生成 proto文件
```bash
(PhxRPC根目录)/codegen/phxrpc_xml2proto -f (xml文件路径) -d .
```

```proto
syntax = "proto3";

package phxbinlogsvr;

message BinLogBuffer
{
    string oldgtid = 1;
    string newgtid = 2;
    bytes eventbuffer = 3;
};

message BinLogResp
{
    repeated int32 section = 1;
    bytes respbuffer = 2;
};

import "google/protobuf/wrappers.proto";
import "google/protobuf/empty.proto";
import "phxrpc/rpc/phxrpc.proto";

service phxbinlog
{
    rpc SendBinLog( google.protobuf.BytesValue ) returns( google.protobuf.Empty ) {
        option( phxrpc.CmdID ) = 1;
        option( phxrpc.OptString ) = "e:";
        option( phxrpc.Usage ) = "-e <emtpy>";
    }

}

```

#### 生成代码


```bash

(PhxRPC根目录)/codegen/phxrpc_pb2server 
-I (PhxRPC根目录) -I (Protobuf include目录) -f (proto文件路径) -d (生成代码放置路径)

#sample
../codegen/phxrpc_pb2server -I ../ -I ../third_party/protobuf/include -f search.proto -d .

调用完工具后，在生成代码放置目录下执行make，即可生成全部的RPC相关代码。
```
