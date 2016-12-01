#include "tinyxml2.h"
#include "xml_proto_render.h"
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
using namespace phxrpc;

void PrintHelp(const char *program) {
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


int main(int argc, char *argv[]) {
	const char *xml_file = NULL;
	const char *dir_path = NULL;

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

	char path[128] = {0};
	strncpy(path, dir_path, sizeof(path));
	if ('/' == path[strlen(path) - 1]) {
		path[strlen(path) - 1] = '\0';
	}

	XmlProtoRender render;
	render.Xml2Proto(argv[0], xml_file, path);
}
