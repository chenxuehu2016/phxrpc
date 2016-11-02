test -f Makefile && make clean

rm -f *.h *.cc *.cpp Makefile *.conf *.proto

../codegen/phxrpc_xml2proto -f phxbinlogsvr.xml -d .
../codegen/phxrpc_pb2server -I ../ -I ../third_party/protobuf/include -f phxbinlogsvr.proto -d .


