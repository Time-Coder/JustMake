#ifndef JUSTMAKE_H
#define JUSTMAKE_H

#include "directory.h"
#include <cstring>

using namespace std;
using namespace dir;

struct Target
{
	string path;
	string name;
	string full_name;
	string base_name;
	string lib_base;
	string expand_name;
	list<Target*> depends;
	bool found = false;

	Target(const string& _full_name);
};

class JustMaker
{
	list<Target> sources;
	list<Target> heads;
	list<Target> libs;
	list<Target> mains;
	
	string CC = "g++";
	string FLAGS = "-std=c++11";
	string LINK_FLAGS = "-std=c++11";
	string BINDIR = ".bin";
	string EXEDIR = ".";

	string EXTERN_INCLUDE;
	string EXTERN_LIBPATH;
	string EXTERN_LIBS;
	string RPATH;

public:
	bool here = false;

private:
	void get_files(string path = ".");
	void read_Makefile();
	bool update_depends(Target& target);
	void write_Makefile();
	void move_to_mains(const string& exes);
	void write_JustMake_build(const string& dest_dir);
	void write_JustMake_Interactive_build(const string& dest_dir);

public:
	JustMaker(){};
	void add_flag(const string& flag);
	void set_flags(const string& flags);
	void set_BinDir(const string& bindir);
	void L(const string& libpath);
	void l(const string& libfile);
	void I(const string& includepath);
	void make(const string& target = "");
	void update();
	void generate();
};

#endif