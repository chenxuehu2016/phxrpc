//
// Created by jackson on 16-12-1.
//

#include "xml_proto_render.h"
#include "tinyxml2.h"

using namespace phxrpc;
using namespace std;
using namespace tinyxml2;

void XmlProtoRender::pb_package(const XMLElement *element, FILE *write) {
	fprintf(write, "syntax = \"proto3\";\n\n");
	fprintf(write, "package %s;\n", element->GetText());

	fprintf(write, "\n");
}

void XmlProtoRender::pb_message(const XMLElement *element, FILE *write) {
	int count = 1;

	fprintf(write, "message %s\n", element->Attribute("name"));
	fprintf(write, "{\n");


	for (const XMLElement *parent = element->FirstChildElement();
	     parent;
	     parent = parent->NextSiblingElement(), count++) {
		if (0 != strcasecmp("field", parent->Name())) {
			printf("pb_message' text ' %s %s is not field\n", parent->Name(), parent->GetText());
			exit(1);
		}
		// repeated
		if (parent->BoolAttribute("repeated")) {
			fprintf(write, "    repeated %s %s = %d;\n", parent->Attribute("type"), parent->Attribute("name"),
			        count);
		} else {
			fprintf(write, "    %s %s = %d;\n", parent->Attribute("type"), parent->Attribute("name"), count);
		}

	}

	fprintf(write, "};\n\n");
}

void XmlProtoRender::pb_enum(const XMLElement *element, FILE *write) {
	fprintf(write, "enum %s\n", element->Attribute("name"));
	fprintf(write, "{\n");


	for (const XMLElement *parent = element->FirstChildElement();
	     parent;
	     parent = parent->NextSiblingElement()) {
		if (0 != strcasecmp("value", parent->Name())) {
			printf("pb_message' text ' %s %s is not field\n", parent->Name(), parent->GetText());
			exit(1);
		}

		fprintf(write, "    %s = %s;\n", parent->Attribute("name"), parent->GetText());
	}

	fprintf(write, "};\n\n");
}

void XmlProtoRender::pb_imports(const XMLElement *element, FILE *write) {
	for (const XMLElement *parent = element->FirstChildElement();
	     parent;
	     parent = parent->NextSiblingElement()) {
		if (0 != strcasecmp("import", parent->Name())) {
			printf("pb_imports' text ' %s %s is not import\n", parent->Name(), parent->GetText());
			exit(1);
		}
		fprintf(write, "import \"%s\";\n", parent->GetText());
	}

	fprintf(write, "\n");
}

void XmlProtoRender::pb_service_rpc(const XMLElement *rpcEle, FILE *write) {
	fprintf(write, "    rpc %s( %s ) returns( %s ) {\n", rpcEle->Attribute("name"), rpcEle->Attribute("paramter"),
	        rpcEle->Attribute("returns"));

	fprintf(write, "        option( phxrpc.CmdID ) = %s;\n", rpcEle->FirstChildElement("cmdID")->GetText());
	fprintf(write, "        option( phxrpc.OptString ) = \"%s\";\n",
	        rpcEle->FirstChildElement("optString")->GetText());
	fprintf(write, "        option( phxrpc.Usage ) = \"%s\";\n", rpcEle->FirstChildElement("usage")->GetText());


	fprintf(write, "    }\n\n");
}

void XmlProtoRender::pb_service(const XMLElement *element, FILE *write) {
	fprintf(write, "service %s\n", element->Attribute("name"));
	fprintf(write, "{\n");

	for (const XMLElement *parent = element->FirstChildElement();
	     parent;
	     parent = parent->NextSiblingElement()) {
		if (0 != strcasecmp("rpc", parent->Name())) {
			printf("pb_service' text ' %s %s is not rpc\n", parent->Name(), parent->GetText());
			exit(1);
		}

		this->pb_service_rpc(parent, write);
	}

	fprintf(write, "}\n\n");
}


string XmlProtoRender::getFileName(const char *filePath) {
	string str(filePath);
	size_t idx1 = 0;
	if (str.find("/") >=0 ) {
		idx1 = str.rfind("/") + 1;
	}

	size_t idx2 = str.length();
	if (str.find("\\.")  >=0 ) {
		idx2 = str.find_last_of(".");
	}

	printf("getFileName' xml file ' %s index: %zd %zd, substr: %s\n", filePath, idx1, idx2,
	       str.substr(idx1, idx2 - idx1).c_str());
	return str.substr(idx1, idx2 - idx1);
}

void XmlProtoRender::GenerateProtoFile(const char *xmlFile, FILE *file) {
	XMLDocument doc;
	doc.LoadFile(xmlFile);
	doc.PrintError();
	XMLElement *packageElement = doc.FirstChildElement("phxrpc")->FirstChildElement("package");
	const char *package = packageElement->GetText();

	for (const XMLElement *parent = doc.FirstChildElement("phxrpc")->FirstChildElement();
	     parent;
	     parent = parent->NextSiblingElement()) {

		if (0 == strcmp("package", parent->Name())) {
			this->pb_package(parent, file);
		} else if (0 == strcasecmp("message", parent->Name())) {
			this->pb_message(parent, file);
		} else if (0 == strcasecmp("imports", parent->Name())) {
			this->pb_imports(parent, file);
		} else if (0 == strcasecmp("service", parent->Name())) {
			this->pb_service(parent, file);
		} else if (0 == strcasecmp("enum", parent->Name())) {
			this->pb_enum(parent, file);
		} else {
			printf("Unkown tag: %s\n", parent->Name());
		}
	}

	printf("xmlfile:%s package is %s\n", xmlFile, package);
}

void XmlProtoRender::Xml2Proto(char *program, const char *xml_file, char *dir_path) {
	char filename[256] = {0};
	const char *tmp = this->getFileName(xml_file).c_str();
	snprintf(filename, sizeof(filename), "%s/%s.proto", dir_path, tmp);

	if (0 != access(xml_file, F_OK)) {
		printf("\n%s: xml file %s is not exist or not access\n", program, xml_file);
		return;
	}


	if (0 != access(filename, F_OK)) {
		FILE *fp = fopen(filename, "w");
		this->GenerateProtoFile(xml_file, fp);
		fclose(fp);
		printf("\n%s: Build %s file ... done\n", program, filename);
	} else {
		printf("\n%s: %s is exist, ip\n", program, filename);
	}
}
