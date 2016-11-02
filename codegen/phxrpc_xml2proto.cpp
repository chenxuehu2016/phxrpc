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
using namespace std;

void PrintHelp(const char * program) {
    printf("\n");
    printf("PHXRPC ProtoBuf tool\n");
    printf("\n");
    printf("%s <-f Xml file> <-d destination file dir> [-v]\n", program);
    printf(" Usage: -f <Xml file>               # Xml File\n");
    printf("        -d <dir>                    # destination file dir\n");
    printf("        -v                          # print this screen\n");
    printf("\n");

    return;
}

void pb_package(const XMLElement *element, FILE *write) {
    fprintf(write, "syntax = \"proto3\";\n\n");
    fprintf(write, "package %s;\n", element->GetText());

    fprintf(write, "\n");
}

void pb_message(const XMLElement *element, FILE *write) {
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
            fprintf(write, "    repeated %s %s = %d;\n", parent->Attribute("type"), parent->Attribute("name"), count);
        } else {
            fprintf(write, "    %s %s = %d;\n", parent->Attribute("type"), parent->Attribute("name"), count);
        }

    }

    fprintf(write, "};\n\n");
}

void pb_enum(const XMLElement *element, FILE *write) {
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

void pb_imports(const XMLElement *element, FILE *write) {
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

void pb_service_rpc(const XMLElement *rpcEle, FILE *write) {
    fprintf(write, "    rpc %s( %s ) returns( %s ) {\n", rpcEle->Attribute("name"), rpcEle->Attribute("paramter"),
            rpcEle->Attribute("returns"));

    fprintf(write, "        option( phxrpc.CmdID ) = %s;\n", rpcEle->FirstChildElement("cmdID")->GetText());
    fprintf(write, "        option( phxrpc.OptString ) = \"%s\";\n",
            rpcEle->FirstChildElement("optString")->GetText());
    fprintf(write, "        option( phxrpc.Usage ) = \"%s\";\n", rpcEle->FirstChildElement("usage")->GetText());


    fprintf(write, "    }\n\n");
}

void pb_service(const XMLElement *element, FILE *write) {
    fprintf(write, "service %s\n", element->Attribute("name"));
    fprintf(write, "{\n");

    for (const XMLElement *parent = element->FirstChildElement();
         parent;
         parent = parent->NextSiblingElement()) {
        if (0 != strcasecmp("rpc", parent->Name())) {
            printf("pb_service' text ' %s %s is not rpc\n", parent->Name(), parent->GetText());
            exit(1);
        }

        pb_service_rpc(parent, write);
    }

    fprintf(write, "}\n\n");
}


string getFileName(const char* filePath) {
    string str(filePath);
    size_t idx1 = 0;
    if (str.find("/")) {
        idx1 = str.rfind("/") + 1;
    }

    size_t idx2 = str.length();
    if (str.find("\\.")) {
        idx2 = str.find_last_of(".");
    }

    printf("getFileName' xml file ' %s index: %zd %zd, substr: %s\n", filePath,  idx1, idx2, str.substr(idx1, idx2 - idx1).c_str());
    return str.substr(idx1, idx2 - idx1 );
}

void GenerateProtoFile(const char *xmlFile, FILE *file) {
    XMLDocument doc;
    doc.LoadFile(xmlFile);
    doc.PrintError();
    XMLElement *packageElement = doc.FirstChildElement("phxrpc")->FirstChildElement("package");
    const char *package = packageElement->GetText();

    for (const XMLElement *parent = doc.FirstChildElement("phxrpc")->FirstChildElement();
         parent;
         parent = parent->NextSiblingElement()) {

        if (0 == strcmp("package", parent->Name())) {
            pb_package(parent, file);
        } else if (0 == strcasecmp("message", parent->Name())) {
            pb_message(parent, file);
        } else if (0 == strcasecmp("imports", parent->Name())) {
            pb_imports(parent, file);
        } else if (0 == strcasecmp("service", parent->Name())) {
            pb_service(parent, file);
        } else if (0 == strcasecmp("enum", parent->Name())) {
            pb_enum(parent, file);
        } else {
            printf("Unkown tag: %s\n", parent->Name());
        }
    }

    printf("xmlfile:%s package is %s\n", xmlFile, package);
}

void Xml2Proto(char *program, const char *xml_file, char* dir_path){
    char filename[256] = {0};
    const char *tmp = getFileName(xml_file).c_str();
    snprintf(filename, sizeof(filename), "%s/%s.proto", dir_path, tmp);


    if (0 != access(xml_file, F_OK)) {
        printf("\n%s: xml file %s is not exist or not access\n", program, xml_file);
        return;
    }


    if (0 != access(filename, F_OK)) {
        FILE *fp = fopen(filename, "w");
        GenerateProtoFile(xml_file, fp);
        fclose(fp);
        printf("\n%s: Build %s file ... done\n", program, filename);
    } else {
        printf("\n%s: %s is exist, ip\n", program, filename);
    }
}


int main(int argc, char * argv[]) {
    const char * xml_file = NULL;
    const char * dir_path = NULL;

    extern char *optarg;
    int c;

    while ((c = getopt(argc, argv, "f:d:v")) != EOF) {
        switch (c) {
            case 'f':
                xml_file = optarg;
                break;
            case 'd':
                dir_path = optarg;
                break;
            default:
                PrintHelp(argv[0]);
                exit(-1);
                break;
        }
    }

    if (NULL == xml_file || NULL == dir_path) {
        printf("Invalid arguments\n");

        PrintHelp(argv[0]);
        exit(0);
    }

    if (0 != access(dir_path, R_OK | W_OK | X_OK)) {
        printf("Invalid dir: %s, errno %d, %s\n\n", dir_path, errno, strerror(errno));
        PrintHelp(argv[0]);
        exit(0);
    }

    char path[128] = { 0 };
    strncpy(path, dir_path, sizeof(path));
    if ('/' == path[strlen(path) - 1]) {
        path[strlen(path) - 1] = '\0';
    }

    Xml2Proto(argv[0], xml_file, path);
}