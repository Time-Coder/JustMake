#include <io.h>
#include <direct.h>
#include <iostream>
#include <fstream>
#include <list>
#include <windows.h>

using namespace std;

struct Target
{
	string path;
	string name;
	string full_name;
	string base_name;
	string lib_base;
	string expand_name;
	list<Target*> depends;

	Target(const string& _full_name);
};

class AutoMaker
{
	list<Target> sources;
	list<Target> heads;
	list<Target> libs;
	
	string CC = "g++";
	string FLAGS = "-std=c++11";
	string LINK_FLAGS = "-std=c++11";
	string BINDIR = ".";

	string EXTERN_INCLUDE;
	string EXTERN_LIBPATH;
	string EXTERN_LIBS;
	string RPATH;

private:
	void get_files(const string& path);
	void get_files();
	void extract_info();
	void update_depends(Target& target);
	void generate();

public:
	AutoMaker();
	void add_flag(const string& flag);
	void set_flags(const string& flags);
	void set_BinDir(const string& bindir);
	void L(const string& libpath);
	void l(const string& libfile);
	void I(const string& includepath);
	void make(const string& target = "");
	void update();
};

void cd(const string& path);
string pwd();