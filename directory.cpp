#include "directory.h"

void dir::format(string& filename)
{
	for(auto it = filename.begin(); it != filename.end();)
	{
		if(*it == '\\')
		{
			*it = '/';
		}

		if(it != filename.begin() && *(it-1) == '/' && *it == '/')
		{
			it = filename.erase(it);
		}
		else
		{
			it++;
		}
	}
	if(filename[0] != '.' && filename[0] != '/' && filename[1] != ':')
	{
		filename = string("./") + filename;
	}
}

vector<string> dir::ls(string dirname)
{
	dir::format(dirname);
	if(dirname[dirname.size()-1] == '/')
	{
		dirname.erase(dirname.size()-1, 1);
	}

    vector<string> filenames;

    struct stat s;
    stat(dirname.c_str(), &s);
    if(!S_ISDIR(s.st_mode))
    {
        return filenames;
    }

    DIR* open_dir = opendir(dirname.c_str());
    if(NULL == open_dir)
    {
        exit(EXIT_FAILURE);
    }
    dirent* p = nullptr;
    while( (p = readdir(open_dir)) != nullptr)
    {
        struct stat st;
        if(p->d_name[0] != '.')
        {
        	if(dirname != ".")
            	filenames.push_back(dirname + "/" + string(p->d_name));
            else
            	filenames.push_back(string(p->d_name));
        }
    }
    closedir(open_dir);
    return filenames;
}

int dir::cd(string dir_name)
{
	dir::format(dir_name);
	return chdir(dir_name.data());
}

string dir::pwd()
{
	char path[1024];
	getcwd(path, 1024);
	string str_path = string(path);
	format(str_path);

	return str_path;
}

std::string dir::username()
{
#ifdef __linux__
    uid_t userid;
    struct passwd* pwd;
    userid = getuid();
    pwd = getpwuid(userid);
    return pwd->pw_name;
#else
    DWORD len = 128;
    char szBuffer[len];
    GetUserName(szBuffer, &len);
    return szBuffer;
#endif
}

bool dir::is_file(string filename)
{
	dir::format(filename);
	if(filename[filename.size()-1] == '/')
	{
		return false;
	}

	struct stat s;
	return (stat(filename.c_str(), &s) == 0 && (s.st_mode & S_IFREG));
}

bool dir::is_dir(string dirname)
{
	dir::format(dirname);
	struct stat s;
	return (stat(dirname.c_str(), &s) == 0 && (s.st_mode & S_IFDIR));
}

bool dir::exist(string name)
{
	dir::format(name);
	struct stat s;
	return (stat(name.c_str(), &s) == 0);
}

int dir::mkdir(string dirname)
{
	dir::format(dirname);
	if(dirname[dirname.size()-1] == '/')
	{
		dirname.erase(dirname.size()-1, 1);
	}

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
		int flag;
		#ifdef __linux__
			flag = ::mkdir(S.top().c_str(), 0755);
		#else
			flag = ::mkdir(S.top().c_str());
		#endif

		if(flag != 0)
		{
			return -1;
		}

		S.pop();
	}

	return 0;
}

int dir::rm(string name)
{
	dir::format(name);

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
	int name_begin = filename.find_last_of("/") + 1;
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
	string path_name = full_name.substr(0, full_name.find_last_of("/\\"));

	return path_name;
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
	format(src);
	format(dest);

	if(!exist(src) || is_file(dest))
	{
		return -1;
	}
	if(dest == src)
	{
		return 1;
	}

	if(src[src.size()-1] == '/')
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
		else if(is_dir(only_path(dest)) && !is_dir(dest) || in_str(name, '.') > 0)
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
		if(exist(dest))
		{
			dest = dest + "/" + only_name(src);
		}
		vector<string> files = ls(src);
		if(-1 == mkdir(dest))
		{
			return -1;
		}
		for(auto it = files.begin(); it != files.end(); it++)
		{
			cp(*it, dest);
		}
	}

	return 0;
}

int dir::mv(string src, string dest)
{
	format(src);
	format(dest);
	if(src == dest)
	{
		return 1;
	}

	int flag = cp(src, dest);
	rm(src);

	return flag;
}

int dir::rename(string src, string dest)
{
	format(src);
	if(src[src.size()-1] == '/')
	{
		src.erase(src.size()-1, 1);
	}

	if(only_name(src) == dest)
	{
		return 1;
	}

	if(!exist(src) || in_str(dest, '/') != -1 || exist(only_path(src) + "/" + dest))
	{
		return -1;
	}

	return ::rename(src.c_str(), (only_path(src) + "/" + dest).c_str());
}

int dir::touch(string filename)
{
	format(filename);
	if(is_file(filename))
	{
		return 1;
	}
	if(is_dir(filename))
	{
		return -1;
	}
	if(-1 == mkdir(only_path(filename)))
	{
		return -1;
	}
	ofstream ofile(filename, ios::binary);
	ofile.close();
	return 0;
}