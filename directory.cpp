#include "directory.h"

string __reverse_slash(string filename)
{
	int n = filename.size();
	for(int i = 0; i < n; i++)
	{
		if(filename[i] == '\\')
		{
			filename[i] = '/';
		}
	}

	return filename;
}

vector<string> dir::ls(const string& dirname)
{
	vector<string> files;
	if(!is_dir(dirname))
	{
		return files;
	}

	long hFile = 0;
	struct _finddata_t fileinfo;
	string p;

	if((hFile = _findfirst(p.assign(dirname).append("/*").c_str(),&fileinfo)) != -1)
	{
		do
		{
			if(strcmp(fileinfo.name,".") != 0 && strcmp(fileinfo.name,"..") != 0)
			{
				files.push_back(p.assign(dirname).append("/").append(fileinfo.name));
			}
		}
		while(_findnext(hFile, &fileinfo)  == 0);
		_findclose(hFile);
	}

	return files;
}

int dir::cd(const string& dir_name)
{
	return chdir(dir_name.data());
}

string dir::pwd()
{
	char path[1024];
	getcwd(path, 1024);
	return __reverse_slash(string(path));
}

string dir::username()
{
	char strBuffer[256] = {0};
	DWORD dwSize = 256;
	GetUserName(strBuffer, &dwSize);

	return string(strBuffer);
}

bool dir::is_file(const string& file_name)
{
	struct stat s;
	return (stat(file_name.c_str(), &s) == 0 && (s.st_mode & S_IFREG));
}

bool dir::is_dir(const string& dir_name)
{
	struct stat s;
	return (stat(dir_name.c_str(), &s) == 0 && (s.st_mode & S_IFDIR));
}

bool dir::exist(const string& name)
{
	struct stat s;
	return (stat(name.c_str(), &s) == 0);
}

int dir::mkdir(const string& dirname)
{
	stack<string> S;
	int n = dirname.size();
	int i = n;
	while(true)
	{
		if(i == n || dirname[i] == '/' || dirname[i] == '\\')
		{
			string current_name = dirname.substr(0, i);
			if(is_dir(current_name))
			{
				break;
			}
			else
			{
				S.push(current_name);
			}
		}
		i--;
		if(i == 0)
		{
			break;
		}
	}
	
	if(S.empty())
	{
		return 1;
	}

	while(!S.empty())
	{
		if(0 != _mkdir(S.top().c_str()))
		{
			return -1;
		}
		S.pop();
	}

	return 0;
}

int dir::rm(const string &name)
{
	if(is_file(name))
	{
		return remove(name.c_str());
	}

	vector<string> files = ls(name);
	if(files.empty())
	{
		return rmdir(name.c_str());
	}

	for(auto it = files.begin(); it != files.end(); it++)
	{
		rm(it->c_str());
	}
	return rmdir(name.c_str());
}

string only_name(const string& filename)
{
	int name_begin = filename.find_last_of("/\\") + 1;
	if(name_begin == filename.npos)
	{
		return filename;
	}
	if(name_begin == filename.size())
	{
		return "";
	}
	return filename.substr(name_begin, filename.size()-name_begin);
}

string only_path(const string& full_name)
{
	return full_name.substr(0, full_name.find_last_of("/\\"));
}

int copy_file_to_dir(const string& filename, const string& dir_name)
{
	if(-1 == dir::mkdir(dir_name))
	{
		return -1;
	}
	if(dir::is_file(dir_name + "/" + only_name(filename)))
	{
		return 1;
	}

	ifstream ifile(filename.c_str(), ios::binary);
	ofstream ofile((dir_name + "/" + only_name(filename)).c_str(), ios::binary);
	ofile << ifile.rdbuf();
	ifile.close();
	ofile.close();

	return 0;
}

int copy_file_to_file(const string& src_filename, const string& dest_filename)
{
	if(-1 == dir::mkdir(only_path(dest_filename)))
	{
		return -1;
	}
	if(dir::is_file(dest_filename))
	{
		return 1;
	}

	ifstream ifile(src_filename.c_str(), ios::binary);
	ofstream ofile(dest_filename.c_str(), ios::binary);
	ofile << ifile.rdbuf();
	ifile.close();
	ofile.close();

	return 0;
}

int in_str(const string& target, char pattern)
{
	int n = target.size();
	for(int i = 0; i < n; i++)
	{
		if(target[i] == pattern)
		{
			return i;
		}
	}
	return -1;
}

int dir::cp(string src, string dest)
{
	if(!exist(src))
	{
		return -1;
	}

	if(is_file(dest))
	{
		return 1;
	}

	src = __reverse_slash(src);
	dest = __reverse_slash(dest);

	while(src[src.size()-1] == '/')
	{
		src.erase(src.end()-1);
	}

	string name = only_name(dest);
	if(is_file(src))
	{
		if(dest[dest.size()-1] == '/')
		{
			return copy_file_to_dir(src, dest);
		}
		else if(is_dir(dest.substr(0, dest.find_last_of("/"))) && !is_dir(dest) || in_str(name, '.') > 0)
		{
			return copy_file_to_file(src, dest);
		}
		else
		{
			return copy_file_to_dir(src, dest);
		}
	}
	else
	{
		vector<string> files = ls(src);
		if(-1 == mkdir(dest + "/" + only_name(src)))
		{
			return -1;
		}
		for(auto it = files.begin(); it != files.end(); it++)
		{
			cp(*it, dest + "/" + only_name(src));
		}
	}

	return 0;
}

int dir::mv(string src, string dest)
{
	bool flag = cp(src, dest);
	rm(src);

	return flag;
}