#include <io.h>
#include <direct.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <sys/stat.h>
#include <cstdio>

#include <stack>
#include <list>
#include <vector>
#include <string>

using namespace std;

namespace dir
{
	int cd(const string& dir_name); // done
	string pwd(); // done
	string username();
	vector<string> ls(const string& dir_name = "."); // done
	int touch(const string& file_name);
	int mkdir(const string& dir_name); // done
	int rm(const string& target_name); // done
	int cp(string src_name, string dest_name); // done
	int mv(string src_name, string dest_name); // done

	bool is_file(const string& filename); // done
	bool is_dir(const string& dirname); // done
	bool exist(const string& name); // done
}