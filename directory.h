#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <stack>

#include <dirent.h>
#include <sys/stat.h>

#ifdef __linux__
	#include <unistd.h>
	#include <pwd.h>
#else
	#include <windows.h>
#endif

using namespace std;

namespace dir
{
	void format(string& filename);

	int cd(string dir_name);
	string pwd();
	string username();
	vector<string> ls(string dir_name = ".");
	int touch(string file_name);
	int mkdir(string dir_name);
	int rm(string target_name);
	int cp(string src_name, string dest_name);
	int mv(string src_name, string dest_name);
	int rename(string src_name, string dest_name);

	bool is_file(string filename);
	bool is_dir(string dirname);
	bool exist(string name);

	string abs_name(const string& filename);
	string abs_path(const string& filename);
}

#endif