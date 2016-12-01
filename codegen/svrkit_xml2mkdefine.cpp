#include "tinyxml2.h"
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <sys/stat.h>	// mkdir
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <vector>
#include <errno.h>
#include <utility>
#include <iostream>
#include <algorithm>
#include <sstream>

#include "syntax_tree.h"

#include "proto_utils.h"
#include "name_render.h"
#include "svrkit_code_render.h"

#include "xml_proto_render.h"

using namespace phxrpc;
using namespace tinyxml2;
using namespace std;

void PrintHelp(const char *program) {
	printf("\n");
	printf("SvrKit Makefile define tool\n");
	printf("\n");
	printf("%s <-f Xml file> <-d destination file dir> [-v]\n", program);
	printf(" Usage: -f <Xml file>               # Xml File\n");
	printf("        -d <dir>                    # destination base dir\n");
	printf("        -v                          # print this screen\n");
	printf("\n");

	return;
}


string getMkDefFileDir(const char *dir_path, const char *pkgName) {
	string str(pkgName);
	if (0 > str.find(":")) {
		return "";
	}
	size_t idx1 = 0;
	size_t idx2 = str.find_last_of(":");

	// printf("getMkDefFileName' pkg name ' %s index: %zd %zd, substr: %s\n", pkgName,  idx1, idx2, str.substr(idx1, idx2 - idx1).c_str());
	string absPath = str.substr(idx1, idx2 - idx1);

	string mk_path(dir_path);
	mk_path.append("/");
	mk_path.append(absPath);

	return mk_path;
}

string getMkDefFileName(const char *base_path, const char *pkgName) {
	const string &file_dir = getMkDefFileDir(base_path, pkgName);
	if (file_dir.empty()) {
		return "";
	}

	string mk_path(file_dir);
	mk_path.append("/Makefile.define");
	return mk_path;
}

string getSimpleName(const char *pkg_name) {
	string str(pkg_name);
	if (0 > str.find(":")) {
		return "";
	}
	size_t idx1 = str.find_last_of(":") + 1;
	size_t idx2 = str.length();

	//printf("getMkDefAllObject' pkg name ' %s index: %zd %zd, substr: %s\n", pkg_name,  idx1, idx2, str.substr(idx1, idx2 - idx1).c_str());
	string simple_name = str.substr(idx1, idx2 - idx1);
	return simple_name;
}

string getMkDefObject(const char *pkg_name, const char *type) {
	string object;

	string simple_name = getSimpleName(pkg_name);
	if (simple_name.empty()) {
		return "";
	}

	string suffix = ".a";
	if (!type) {
		object.append("lib");
	} else if (strcasecmp(type, "bin") == 0) {
		suffix = "";
	} else if (strcasecmp(type, "elib") == 0) {
		object.append("elib");
	} else {
		object.append("lib");
	}

	object.append(simple_name);
	object.append(suffix);
	return object;
}

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
	stringstream ss;
	ss.str(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
}

// trim from start
static inline std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
	                                std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(),
	                     std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
	return ltrim(rtrim(s));
}

// mkdir http://stackoverflow.com/questions/2336242/recursive-mkdir-system-call-on-unix
static void _mkdir(const char *dir) {
	char tmp[256];
	char *p = NULL;
	size_t len;

	snprintf(tmp, sizeof(tmp), "%s", dir);
	len = strlen(tmp);
	if (tmp[len - 1] == '/')
		tmp[len - 1] = 0;
	for (p = tmp + 1; *p; p++)
		if (*p == '/') {
			*p = 0;
			mkdir(tmp, S_IRWXU);
			*p = '/';
		}
	mkdir(tmp, S_IRWXU);
}

string GetObjByCfiles(const char *cfiles) {
	string tmp(cfiles);
	string obj_str(" ");
	vector<string> elems;

	split(tmp, ',', elems);

	for (string ele : elems) {
		obj_str.append(trim(ele)).append(".o ");
	}
	return obj_str;
}

XMLNode *deepCopy( XMLNode *src, XMLDocument *destDoc )
{
	XMLNode *current = src->ShallowClone( destDoc );
	for( XMLNode *child=src->FirstChild(); child; child=child->NextSibling() )
	{
		current->InsertEndChild( deepCopy( child, destDoc ) );
	}

	return current;
}

const XMLElement* getPkgElementByAttr(const char* program, const XMLElement *pkgs, const char* attr) {
	const XMLElement *pb_ele = 0;
	for (const XMLElement* child = pkgs->FirstChildElement();
	     child;
	     child = child->NextSiblingElement()) {
		const char *name = child->Attribute("name");
		if (name != 0 && 0 == strcmp(attr, name)) {
			pb_ele = child;
			// printf("\n%s: Generate Phxrpc OBJ 'element':%s is found, %s \n", program, name, attr);
			break;
		}
	}
	return pb_ele;
}

string GetObjByPhxrpc(const char* program, const XMLElement *element, const XMLElement *pkgs) {
	const char *phxrpc, *pb_ref, *pbdir, *pbname, *pbfile;
	string buffer;
	NameRender name_render("");
	SvrKitCodeRender codeRender(name_render);

	phxrpc = element->Attribute("phxrpc");
	if (0 == phxrpc) {
		return "";
	}
	pb_ref = element->Attribute("protobuf-ref");
	if (0 == pb_ref) {
		printf(
		"\n%s: Get Phxrpc OBJ paramter error from 'element':%s, 'protobuf-ref':%s  is empty \n",
		program, element->Attribute("name"), pb_ref);
		exit(1);
	}

	const XMLElement *pb_ele = getPkgElementByAttr(program, pkgs, pb_ref);
	if (0 == pb_ele) {
		printf("\n%s: Get Phxrpc OBJ 'element':%s is not found \n", program, pb_ref);
		exit(1);
	}

	pbdir = pb_ele->Attribute("dir");
	pbfile = pb_ele->Attribute("file");
	pbname = pb_ele->Attribute("name");

	if (0 == pbfile || 0 == pbfile || 0 == pbname) {
		printf("\n%s: Generate Phxrpc OBJ paramter error from 'element':%s, 'pbname':%s 'pbfile':[%s] 'pbdir':[%s] is empty \n",
		program, pb_ele->Attribute("name"), pbname, pbfile, pbdir);
		exit(1);
	}

	if (0 == strcasecmp(phxrpc, "client")) {
		buffer.append(codeRender.GetPhxrpcClientObjs(pbname, pbdir, pbfile));
	} else if (0 == strcasecmp(phxrpc, "svr")) {
		buffer.append(codeRender.GetPhxrpcServerObjs(pbname, pbdir, pbfile));
	} else if (0 == strcasecmp(phxrpc, "tools")) {
		buffer.append(codeRender.GetPhxrpcToolsObjs(pbname, pbdir, pbfile));
	} else if (0 == strcasecmp(phxrpc, "protobuf")) {
		buffer.append(codeRender.GetPhxrpcMessageObjs(pbname, pbdir, pbfile));
	}
	return buffer;
}

string GetObj(const char* program, const XMLElement *element, const XMLElement *pkgs) {
	//cout << "cfiles:" << element->Attribute("cfiles") << ",";
	//cout << "proto:" << element->Attribute("proto") << ",";
	//cout << "phxrpc:" << element->Attribute("phxrpc") << endl;


	const char *cfiles = element->Attribute("cfiles");
	if (0 != cfiles) {
		return GetObjByCfiles(cfiles);
	}

	const char *phxrpc = element->Attribute("phxrpc");
	if (0 != phxrpc) {
		return GetObjByPhxrpc(program, element, pkgs);
	}
	return "";
}

string GetTextPrefix(const XMLElement *parent) {
	string str;
	const char *prefix = parent->Attribute("prefix");
	if (0 != prefix) {
		str.append(prefix);
	}
	const char *text = parent->GetText();
	if (0 == text) {
		return str;
	}

	if (0 != prefix) {
		str.append("/");
	}
	str.append(text);
	return str;
}

string GetPhxrpcVpath(const char *program, const XMLElement *element, const char *base_path, const XMLElement *pkgs) {
	const char *phxrpc, *pb_ref, *pbdir, *pbname, *pbfile;
	string buffer;
	NameRender name_render("");
	SvrKitCodeRender codeRender(name_render);

	phxrpc = element->Attribute("phxrpc");
	if (0 == phxrpc) {
		return "";
	}

	pb_ref = element->Attribute("protobuf-ref");
	if (0 == pb_ref) {
		printf(
		"\n%s: Generate Phxrpc OBJ paramter error from 'element':%s, 'protobuf-ref':%s  is empty \n",
		program, element->Attribute("name"), pb_ref);
		exit(1);
	}

	const XMLElement *pb_ele = getPkgElementByAttr(program, pkgs, pb_ref);
	if (0 == pb_ele) {
		printf("\n%s: Generate Phxrpc OBJ 'element':%s is not found \n", program, pb_ref);
		exit(1);
	}

	pbdir = pb_ele->Attribute("dir");
	pbfile = pb_ele->Attribute("file");
	pbname = pb_ele->Attribute("name");

	if (0 == pbfile || 0 == pbdir || 0 == pbname) {
		printf(
		"\n%s: Generate Phxrpc vpath paramter error from 'element':%s, 'pbdir':%s 'pbfile':[%s] 'pbname':[%s] is empty \n",
		program, element->Attribute("name"), pbdir, pbfile, pbname);
		exit(1);
	}

	if (0 == strcasecmp(phxrpc, "client")) {
		buffer.append(codeRender.GetPhxrpcClientVpath(pbname, pbdir, pbfile));
	} else if (0 == strcasecmp(phxrpc, "svr")) {
		buffer.append(codeRender.GetPhxrpcServerVpath(pbname, pbdir, pbfile));
	} else if (0 == strcasecmp(phxrpc, "tools")) {
		buffer.append(codeRender.GetPhxrpcToolsVpath(pbname, pbdir, pbfile));
	}
	return buffer;
}

int GenerateHppFile(const char *base_path, const char *abs_path, const char *simplename) {

	string mk_path, mk_dir;
	mk_dir.append(base_path).append("/").append(abs_path).append("/");
	mk_path.append(mk_dir).append(simplename);

	const char *filename = mk_path.c_str();

	if (0 != access(filename, F_OK)) {
		FILE *fp = fopen(filename, "w");
		if (NULL == fp) {
			_mkdir(mk_dir.c_str());
			fp = fopen(filename, "w");
		}
	} else {
		printf("\nGenerateHppFile filename:'%s' is exist, base_path:%s, abs_path:%s\n", filename, base_path, abs_path);
	}

	return 1;
}

int GenerateCppFile(const char *base_path, const char *abs_path, const char *simplename) {
	return 1;
}

void GenerateHppCppFiles(const char *program, const char *base_path, const XMLElement *element) {
	bool is_cur_dir = false, mv_inc_all = false, mv_inc_some = false;
	int count_h = 0, count_cpp = 0;
	vector<string> elems;

	const char *name = element->Attribute("name");
	const string &mk_dir = getMkDefFileDir(base_path, name);
	const char *cfiles = element->Attribute("cfiles");
	const char *hinc = element->Attribute("hinc");
	const char *hmvfiles = element->Attribute("hmvfiles");

	if (0 == cfiles) {
		// 不需要生成
		return;
	}

	if (0 == hinc && 0 == hmvfiles) {
		is_cur_dir = true;
	} else if (0 != hinc && 0 == hmvfiles) {
		mv_inc_all = true;
	} else if (0 != hinc && 0 != hmvfiles) {
		mv_inc_some = true;
	} else {
		printf("\n%s: GenerateHppCppFiles 'cfiles':[%s], 'hinc':%s is not exist, but 'hmvfiles':[%s] is exist \n",
		       program, cfiles, hinc, hmvfiles);
		exit(1);
	}

	string tmp(cfiles);
	split(tmp, ',', elems);

	for (string simplename : elems) {

		if (is_cur_dir) {                   // .h 和 .cpp在一起
			count_h += GenerateHppFile(base_path, mk_dir.c_str(), simplename.c_str());
		} else if (mv_inc_all) {             // 所有 .h 移动到 hinc 目录
			count_h += GenerateHppFile(base_path, hinc, simplename.c_str());
		} else if (mv_inc_some &&           // 部分 .h 移动到 hinc 目录
		           NULL != strstr(hmvfiles, simplename.c_str())) {
			count_h += GenerateHppFile(base_path, hinc, simplename.c_str());
		}

		// 生成 .cpp
		count_cpp += GenerateCppFile(base_path, mk_dir.c_str(), simplename.c_str());
	}
}


// 根据 xml生成 protobuf
void GenerateProtobufFiles(const char* program,
                        const XMLElement *element, const XMLElement *pkgs,
                        const char *base_path, const char *xml_dir) {

	const char *phxrpc, *pb_ref, *pbdir, *pbname, *pbfile;
	string schema_xml, protobuf_dir;

	phxrpc = element->Attribute("phxrpc");
	if (0 == phxrpc) {
		return ;
	}
	pb_ref = element->Attribute("protobuf-ref");
	if (0 == pb_ref) {
		printf(
		"\n%s: Generate Phxrpc Source File: paramter error from 'element':%s, 'protobuf-ref':%s  is empty \n",
		program, element->Attribute("name"), pb_ref);
		exit(1);
	}

	const XMLElement *pb_ele = getPkgElementByAttr(program, pkgs, pb_ref);
	if (0 == pb_ele) {
		printf("\n%s:  Generate Phxrpc Source File: 'element':%s is not found \n", program, pb_ref);
		exit(1);
	}

	pbdir = pb_ele->Attribute("dir");
	pbfile = pb_ele->Attribute("file");
	pbname = pb_ele->Attribute("name");
	const XMLElement *ref_ele = pb_ele->LastChildElement("ref");

	if (0 == pbfile || 0 == pbfile || 0 == pbname) {
		printf("\n%s: Generate Phxrpc Source File: paramter error from 'element':%s, 'pbname':%s 'pbfile':[%s] 'pbdir':[%s] is empty \n",
		       program, pb_ele->Attribute("name"), pbname, pbfile, pbdir);
		exit(1);
	}

	if (0 == ref_ele) {
		printf("\n%s: Generate Phxrpc Source File: the child <ref> of 'element':%s is not found \n",
		       program, pb_ele->Attribute("name"));
		exit(1);
	}

	schema_xml.append(xml_dir);
	if ('/' != xml_dir[strlen(xml_dir) - 1]) {
		schema_xml.append("/");
	}
	schema_xml.append(ref_ele->Attribute("file"));

	protobuf_dir.append(base_path);
	if ('/' != base_path[strlen(base_path) - 1]) {
		protobuf_dir.append("/");
	}
	protobuf_dir.append(pbdir);

	char path[128] = {0};
	XmlProtoRender render;

	strncpy(path, protobuf_dir.c_str(), sizeof(path));
	if ('/' == path[strlen(path) - 1]) {
		path[strlen(path) - 1] = '\0';
	}

	if (0 == strcasecmp(phxrpc, "protobuf")) {
		//	render.Xml2Proto("phxrpc_xml2proto", schema_xml.c_str(), "/data/tmp/phxrpc/tmp3/phxbinlogsvr/framework/proto/");
		render.Xml2Proto("phxrpc_xml2proto", schema_xml.c_str(), path);
	}
}


void GenerateMkDefine(const char *program, FILE *write,
                      const XMLElement *element, const XMLElement *pkgs,
                      const char *base_path) {
	int count = 1;

	const char *pkgName = element->Attribute("name");
	const char *type = element->Attribute("type");
	string simple_name = getSimpleName(pkgName);
	string upper_name(simple_name), lib, syslib, inc, cppflags, vpath;

	transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);
	// cout << "simple_name:" << simple_name << ", upper_name" << upper_name << endl;

	for (const XMLElement *parent = element->FirstChildElement();
	     parent;
	     parent = parent->NextSiblingElement(), count++) {

		const XMLElement *lastEle = element->LastChildElement(parent->Name());
		bool isEnd = parent == lastEle;

		const char *text = parent->GetText();
		if (0 == strcasecmp("lib", parent->Name())) {
			if (0 != text) {
				lib.append(text);
				if (!isEnd) lib.append(" ");
			}
		} else if (0 == strcasecmp("syslib", parent->Name())) {
			syslib.append(GetTextPrefix(parent));

			if (!isEnd) syslib.append(" ");
		} else if (0 == strcasecmp("inc", parent->Name())) {
			inc.append(GetTextPrefix(parent));

			if (!isEnd) inc.append(" ");
		} else if (0 == strcasecmp("extra-cppflags", parent->Name())) {

			if (0 != text) {
				cppflags.append(text);
				if (!isEnd) cppflags.append(" ");
			}
		} else {
			printf("pkg' text ' %s %s is not field\n", parent->Name(), text);
			exit(1);
		}
	}

	fprintf(write, "allobject= %s\n\n", getMkDefObject(pkgName, type).c_str());

	fprintf(write, "%s_OBJ=%s\n\n", upper_name.c_str(), GetObj(program, element, pkgs).c_str());

	fprintf(write, "%s_LIB=%s\n\n", upper_name.c_str(), lib.c_str());

	fprintf(write, "%s_SYS_LIB=%s\n\n", upper_name.c_str(), syslib.c_str());

	fprintf(write, "%s_INCS=%s\n\n", upper_name.c_str(), inc.c_str());

	fprintf(write, "%s_EXTRA_CPPFLAGS=%s\n\n", upper_name.c_str(), cppflags.c_str());

	vpath = GetPhxrpcVpath(program, element, base_path, pkgs);
	fprintf(write, "%s\n\n", vpath.c_str());

}

int pkg_lib(const char *program,
            const XMLElement *element,const XMLElement *pkgs,
            const char *base_path, const char *xml_dir) {

	int count = 0;
	const char *name = element->Attribute("name");

	const string &mk_path = getMkDefFileName(base_path, name);
	const string &mk_dir = getMkDefFileDir(base_path, name);

	const char *filename = mk_path.c_str();

	// if (0 != access(filename, F_OK)) {
	FILE *fp = fopen(filename, "w");
	if (NULL == fp) {
		_mkdir(mk_dir.c_str());
		fp = fopen(filename, "w");
	}
	printf("\n%s: open file to %s file ... done\n", program, filename);
	GenerateMkDefine(program, fp, element, pkgs, base_path);
	fclose(fp);

	count = 1;

	printf("\n%s: element '%s' to '%s' file ... done\n", program, name, filename);
	// } else {
	//      printf("\n%s: element '%s' to '%s' is exist\n", program, name, filename);
	// }

	//GenerateHppCppFiles(program, base_path, element);
	GenerateProtobufFiles(program, element, pkgs, base_path, xml_dir);

	return count;
}

string getFileName(const char *filePath) {
	string str(filePath);
	size_t idx1 = 0;
	if (0 <= str.find("/")) {
		idx1 = str.rfind("/") + 1;
	}

	size_t idx2 = str.length();
	if (0 <= str.find("\\.")) {
		idx2 = str.find_last_of(".");
	}

	//printf("getFileName' xml file ' %s index: %zd %zd, substr: %s\n", filePath,  idx1, idx2, str.substr(idx1, idx2 - idx1).c_str());
	return str.substr(idx1, idx2 - idx1);
}


void GenerateSrcList(char *program, const XMLElement *element, FILE *write) {
	int count = 1;
	string dirs;

	for (const XMLElement *parent = element->FirstChildElement();
	     parent;
	     parent = parent->NextSiblingElement(), count++) {
		if (0 != strcasecmp("dir", parent->Name())) {
			printf("GenerateSrcList' text ' %s %s is not field\n", parent->Name(), parent->GetText());
			exit(1);
		}
		dirs.append(parent->GetText());
		dirs.append(" ");
	}

	fprintf(write, "%s\n", dirs.c_str());
}

int src_list(char *program, const XMLElement *element, const char *base_dir) {
	const char *name = element->Attribute("name");
	int count = 0;

	string src_list(base_dir);
	src_list.append("/");
	src_list.append(name);
	src_list.append("/");
	src_list.append("src_list");

	const char *filename = src_list.c_str();
	const string &mk_dir = getMkDefFileDir(base_dir, name);

	//	if (0 != access(filename, F_OK)) {
	FILE *fp = fopen(filename, "w");
	if (NULL == fp) {
		_mkdir(mk_dir.c_str());
		fp = fopen(filename, "w");
	}
	GenerateSrcList(program, element, fp);
	fclose(fp);
	printf("\n%s: element %s to %s file ... done\n", program, name, filename);
	count = 1;
	//	} else {
	//		printf("\n%s: element %s to %s is exist\n", program, name, filename);
	//	}

	return count;
}

void ReadXml2MkDefine(char *program, const char *xmlFile,const char *base_path) {
	int count = 0, child_count = 0;
	string filename(xmlFile);

	XMLDocument doc;
	doc.LoadFile(xmlFile);
	doc.PrintError();
	XMLElement *pkgs = doc.FirstChildElement("svrkit")->FirstChildElement("pkgs");

	printf("\n%s: parsing xmlfile:%s to create Makefile.define ...\n", program, xmlFile);

	// http://stackoverflow.com/questions/8518743/get-directory-from-file-path-c
	string xml_dir;
	const size_t last_slash_idx = filename.rfind('/');
	if (std::string::npos != last_slash_idx)
	{
		xml_dir = filename.substr(0, last_slash_idx + 1);
	}

	printf("\n%s: the dir of xmlfile:[%s] is [%s] ...\n", program, xmlFile, xml_dir.c_str());

	// FILE *file
	for (const XMLElement *parent = pkgs->FirstChildElement();
	     parent;
	     parent = parent->NextSiblingElement()) {
		child_count++;

		if (0 == strcmp("pkg", parent->Name())) {
			//FILE *file =
			count += pkg_lib(program, parent, pkgs, base_path, xml_dir.c_str());
		} else if (0 == strcasecmp("src-list", parent->Name())) {
			count += src_list(program, parent, base_path);
		} else {
			printf("Unkown tag: %s\n", parent->Name());
		}
	}

	printf("\n%s: xmlfile:%s(%d child elements) generate %d files, success!!\n", program, xmlFile, child_count, count);
}


void Xml2MkDefine(char *program, const char *xml_file, char *dir_path) {
	if (0 != access(xml_file, F_OK)) {
		printf("\n%s: xml file %s is not exist or not access\n", program, xml_file);
		return;
	}

	ReadXml2MkDefine(program, xml_file, dir_path);
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
		printf("Invalid base dir: %s, errno %d, %s\n\n", dir_path, errno, strerror(errno));
		PrintHelp(argv[0]);
		exit(0);
	}

	char path[128] = {0};
	strncpy(path, dir_path, sizeof(path));
	if ('/' == path[strlen(path) - 1]) {
		path[strlen(path) - 1] = '\0';
	}

	Xml2MkDefine(argv[0], xml_file, path);
}