//
// Created by jackson on 16-12-1.
//

#ifndef PHXRPC_XML_PROTO_RENDER_H
#define PHXRPC_XML_PROTO_RENDER_H

#include "tinyxml2.h"
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <sys/stat.h>	// mkdir
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <errno.h>

using namespace tinyxml2;

namespace phxrpc {
	class XmlProtoRender {
	public:
		void Xml2Proto(char *program, const char *xml_file, char *dir_path);


	private:
		void pb_package(const XMLElement *element, FILE *write);

		void pb_message(const XMLElement *element, FILE *write);

		void pb_enum(const XMLElement *element, FILE *write);

		void pb_imports(const XMLElement *element, FILE *write);

		void pb_service_rpc(const XMLElement *rpcEle, FILE *write);

		void pb_service(const XMLElement *element, FILE *write);

		std::string getFileName(const char *filePath);

		void GenerateProtoFile(const char *xmlFile, FILE *file);
	};
}


#endif //PHXRPC_XML_PROTO_RENDER_H
