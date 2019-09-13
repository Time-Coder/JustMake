#include "directory.h"
#include <list>

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
	string BINDIR = ".";

	string EXTERN_INCLUDE;
	string EXTERN_LIBPATH;
	string EXTERN_LIBS;
	string RPATH;

public:
	bool here = false;

private:
	void get_files(const string& path);
	void get_files();
	void extract_info();
	bool update_depends(Target& target);
	void write_JustMakefile();
	void move_to_mains(const string& exes);

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