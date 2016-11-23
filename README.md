<img align="right" src="http://mmbiz.qpic.cn/mmbiz/UqFrHRLeCAkOcYOjaX3oxIxWicXVJY0ODsbAyPybxk4DkPAaibgdm7trm1MNiatqJYRpF034J7PlfwCz33mbNUkew/640?wx_fmt=jpeg&wxfrom=5&wx_lazy=1" hspace="15" width="150px" style="float: right">

**PhxRPC是微信后台团队推出的一个非常简洁小巧的RPC框架，编译生成的库只有450K。**

**更多文档[官方文档(https://github.com/tencent-wechat/phxrpc/blob/master/README.md)**

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

# phxrpc 构建编译管理方式
参考[phxsql](https://github.com/tencent-wechat/phxsql) 中使用自定义`Makefile.define`文件，然后使用python脚本`tools/create_makefile.py`生成`Makefile`


下面是`phxsql`项目模块`phxbinlogsvr/config`的配置[Makefile.define](https://github.com/tencent-wechat/phxsql/blob/master/phxbinlogsvr/config/Makefile.define)

```Makefile.define
allobject= libphxbinlogconfig.a

PHXBINLOGCONFIG_OBJ= phxbinlog_svr_base_config.o phxsql_config.o phxbinlog_config.o 

PHXBINLOGCONFIG_LIB=phxbinlogconfig phxcomm/configparser:phxconfig phxcomm/utils:phxutils include:phxcomm_include

PHXBINLOGCONFIG_SYS_LIB=

PHXBINLOGCONFIG_INCS=$(SRC_BASE_PATH)/phxbinlogsvr/config

PHXBINLOGCONFIG_EXTRA_CPPFLAGS=-Wall
```
生成了的Makefile文件， 非常复杂， 这里就不举例了。

假如一个项目有两三个模块，一个模块有四五个子模块，每个模块之间有一两个依赖。
估算一下个关系 = 3模块 × 5子模块 × 2相互依赖 = 30，维护这三个关系。
而且这些关系还在不同15个不同的目录下， 这样维护这些`Makefiel.define`将变得非常困难。

若能在一个文件完成所有配置， 并且自动生成所有的`Makefile.define`， 就能够解决上面的问题。更多细节，请参考下面关于 xml2mkdefine 的章节



# 如何使用
#### 编写xml文件
下面是sample目录下的xml文件样例。

```xml
<?xml version="1.0"?>
<!-- sudo apt install libxml2-utils; xmllint &#45;&#45;noout &#45;&#45;schema ../schema.xsd phxbinlogsvr.xml -->
<phxrpc xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
        xsi:noNamespaceSchemaLocation="./schema.xsd">
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
```
调用完工具后，在生成代码放置目录下执行make，即可生成全部的RPC相关代码。

# svrkit 全局管理Makefile

## 怎么全局管理生Makefile？ 
就是在一个的配置文件集中配置， 配置模块的编译类型、包括哪些cpp源文件件列表、它的头文件在哪个目录、
依赖项目内静态库、依赖哪些系统库、有什么cpp-flags， 还有属于phxrpc那些模块。
 
编译类型包括： 静态链接库libxxx.a, 头文件静态库elibxxx.a, 可执行二进制文件bin
属于phxrpc那些模块： 按功能把phxrpc分为4个子功能： client, svr, tools, protobuf，把4个子功能分在不同的子模块，方便依赖管理。
比如client子功能会被其他模块依赖，tools和svr直接生成bin文件。
 
## 配置文件svrkit.xml

继续使用上面模块`phxbinlogsvr/config`的配置来举例

```xml
<?xml version="1.0"?>
<svrkit  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:noNamespaceSchemaLocation="./svrkit.xsd">

    <pkgs>
        <pkg name="phxbinlogsvr/config:phxbinlogconfig" hinc="include/phxbinlogsvr/config"
             cfiles="phxbinlog_svr_base_config, phxsql_config, phxbinlog_config">
            <!--PHXBINLOGCONFIG_LIB=phxbinlogconfig phxcomm/configparser:phxconfig phxcomm/utils:phxutils include:phxcomm_include-->
            <lib>phxbinlogconfig</lib>
            <lib>phxcomm/configparser:phxconfig</lib>
            <lib>phxcomm/utils:phxutils</lib>
            <lib>include:phxcomm_include</lib>
            <syslib></syslib>
            <!--PHXBINLOGCONFIG_INCS=$(SRC_BASE_PATH)/phxbinlogsvr/config-->
            <inc prefix="$(SRC_BASE_PATH)">phxbinlogsvr/config</inc>
            <!--PHXBINLOGCONFIG_EXTRA_CPPFLAGS=-Wall-->
            <extra-cppflags>-Wall</extra-cppflags>
        </pkg>
    </pkgs>
</svrkit>

```

* 配置文件schema svrkit.xsd 用于检查正确性， 使用命令检查
```
# ubuntu
#sudo apt install libxml2-utils;
xmllint --noout --schema ./svrkit.xsd svrkit.xml
```
* 标签&lt;pkgs&gt;， 所有模块都写在这个标签里

* 标签&lt;pkg&gt;， 编写的模块的地方

| 属性           | 名称     | 必须 |  描述  |
| ---- |:--------:|------ | -----|-----|
| name | 模块全名  |  是    | "相对路径" + ":" + "模块简称"， 例如： `phxbinlogsvr/config:phxbinlogconfig`   |
| type | 编译类型  | 否     |静态链接库libxxx.a, 头文件静态库elibxxx.a, 可执行二进制文件xxxbin; 默认是前者, "xxx"是模块简称 |
| cfiles| cpp源文件件列表 | 否 | cpp源文件， 不包括文件名后缀，以","隔开；例如: `phxbinlog_svr_base_config, phxsql_config` |
| hinc  |  cpp头文件目录  | 否  |   头文件目录， 默认是模块的当前目录 |
| hmvfiles      |  部分cpp头文件列表  | 否  |  按列表移动到头文目录`hinc` |
| phxrpc | phxrpc子功能    | 否  |  包括子功能： client, svr, tools, protobuf |
| proto |  protobuf文件    | 否 | proto文件， 相对路径， 例如： `phxbinlogsvr/framework/proto/phxbinlogsvr.proto` |

| 子标签/属性     | 名称           | 允许重复  |描述  |
| ------------- |:-------------:| -----|-----|
| &lt;lib&gt; |   项目内静态库     |  允许 | 依赖其他pkg name， 例如： `phxbinlogconfig`， `phxcomm/configparser:phxconfig` |
| &lt;syslib prefix=""&gt; | 系统库  |  允许   |   属性prefix是， 路径前缀 |
| &lt;inc prefix=""&gt; |  编译include目录 |  允许| 例如： `<syslib prefix="$(PHXRPC_LIB_PATH)">libphxrpc.a</syslib>` |
| &lt;extra-cppflags&gt;| 额外的cpp-flags | 不 |  例如： `-Wall -Werror` |
| &lt;vpath prefix&gt; |  phxrpc相关的vpath    |  不 | 例如： `` |


## 生成Makefile.define文件
```
mkdir tmp
cd tmp
../codegen/svrkit_xml2mkdefine -f ../svrkit.xml -d .
```
## 生成Makefile
```
./tools/create_makefile.py .
```
