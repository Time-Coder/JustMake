#include "automake.h"

string _reverse_slash(string filename)
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

bool has_file(const string& filename)
{
	ifstream ifile(filename.data());
	return ifile.good();
}

bool has_dir(const string& dirname)
{
	return (_access(dirname.c_str(), 0) != -1);
}

bool has_makefile(const string& path)
{
	return has_file("Makefile") || has_file("makefile") || has_file("AutoMakefile");
}

bool is_root(const string& path)
{
	int n = path.size();
	int n_slash = 0;
	for(int i = 0; i < n; i++)
	{
		if(path[i] == '/')
		{
			n_slash++;
		}
	}
	return (n_slash <= 1);
}

long long modify_time(const string& filename)
{
    FILETIME t_create, t_access, t_write;

    HANDLE hFile = CreateFile(filename.data(), 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    GetFileTime(hFile, &t_create, &t_access, &t_write);
    CloseHandle(hFile);

    return *((long long*)(&t_write));
}

void delete_space(string& str)
{
	int n = str.size();
	for(int i = 0; i < n; i++)
	{
		if(str[i] == ' ')
		{
			str.erase(i, 1);
			n--;
		}
	}
}

list<Target>::iterator find_full_name(list<Target>& files, const string& full_name)
{
	list<Target>::iterator it = files.begin();
	for(; it != files.end(); it++)
	{
		if(it->full_name == full_name)
		{
			break;
		}
	}
	return it;
}

list<Target>::iterator find_name(list<Target>& files, const string& name)
{
	list<Target>::iterator it = files.begin();
	for(; it != files.end(); it++)
	{
		if(it->name == name)
		{
			break;
		}
	}
	return it;
}

bool found(const list<string>& str_list, const string& str)
{
	for(auto it = str_list.begin(); it != str_list.end(); it++)
	{
		if(*it == str)
		{
			return true;
		}
	}
	return false;
}

Target::Target(const string& _full_name) : full_name(_full_name)
{
	_reverse_slash(_full_name);

	int slash = full_name.find_last_of('/');
	path = full_name.substr(0, slash);
	name = full_name.substr(slash + 1, full_name.size() - slash - 1);

	int point = name.find_last_of('.');
	base_name = name.substr(0, point);
	expand_name = name.substr(point + 1, name.size() - point - 1);

	int lib = base_name.find_first_of("lib");
	if(lib == 0)
	{
		lib_base = base_name.substr(3, base_name.size() - 3);
	}
}

void AutoMaker::write_AutoMakefile()
{
	ofstream AutoMakefile("AutoMakefile");

	AutoMakefile << "CC = " << CC << endl;
	AutoMakefile << "FLAGS = " << FLAGS << endl;
	AutoMakefile << "LINK_FLAGS = " << LINK_FLAGS << endl;
	AutoMakefile << "BINDIR = " << BINDIR << endl << endl;

	AutoMakefile << "EXTERN_INCLUDE = \\\n" << EXTERN_INCLUDE << endl;
	AutoMakefile << "EXTERN_LIBPATH = \\\n" << EXTERN_LIBPATH << endl;
	AutoMakefile << "EXTERN_LIBS = \\\n" << EXTERN_LIBS << endl;

	string last_path = "";
	AutoMakefile << "INCLUDE = \\" << endl;
	for(auto it = heads.begin(); it != heads.end(); it++)
	{
		if(it->path != last_path)
		{
			AutoMakefile << "-I" << it->path << " \\" << endl;
		}
		last_path = it->path;
	}
	AutoMakefile << endl;

	last_path = "";
	AutoMakefile << "LIBPATH = \\" << endl;
	for(auto it = libs.begin(); it != libs.end(); it++)
	{
		if(it->path != last_path)
		{
			AutoMakefile << "-L" << it->path << " \\" << endl;
		}
		last_path = it->path;
	}
	AutoMakefile << endl;

	AutoMakefile << "LIBS = \\" << endl;
	for(auto it = libs.begin(); it != libs.end(); it++)
	{
		AutoMakefile << "-l" << it->lib_base << " \\" << endl;
	}
	AutoMakefile << endl;

	AutoMakefile << "OBJS = \\" << endl;
	for(auto it = sources.begin(); it != sources.end(); it++)
	{
		AutoMakefile << "$(BINDIR)/" << it->base_name << ".o \\" << endl;
	}
	AutoMakefile << endl;

	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		AutoMakefile << "DEPENDS_" << it->base_name << " = \\" << endl
					 << it->full_name << " \\" << endl;
		for(auto it_depends = it->depends.begin(); it_depends != it->depends.end(); it_depends++)
		{
			AutoMakefile << (*it_depends)->full_name << " \\" << endl;
		}
		AutoMakefile << endl;
	}

	for(auto it = sources.begin(); it != sources.end(); it++)
	{
		AutoMakefile << "DEPENDS_" << it->base_name << " = \\" << endl
					 << it->full_name << " \\" << endl;
		for(auto it_depends = it->depends.begin(); it_depends != it->depends.end(); it_depends++)
		{
			AutoMakefile << (*it_depends)->full_name << " \\" << endl;
		}
		AutoMakefile << endl;
	}

	if(!RPATH.empty())
	{
		AutoMakefile << "PATH := " << RPATH << "$(PATH)" << endl << endl;
	}

	AutoMakefile << "all: ";
	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		AutoMakefile << "$(BINDIR)/" << it->base_name << ".exe ";
	}
	AutoMakefile << endl << endl;

	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		AutoMakefile << "$(BINDIR)/" << it->base_name << ".exe: $(BINDIR)/" << it->base_name << ".o $(OBJS)" << endl;
		AutoMakefile << "\t$(CC) $(LINK_FLAGS) $(BINDIR)/" << it->base_name << ".o $(OBJS) -o $(BINDIR)/" << it->base_name << ".exe" << " $(LIBPATH) $(EXTERN_LIBPATH) $(LIBS) $(EXTERN_LIBS)" << endl << endl;
	}

	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		AutoMakefile << "$(BINDIR)/" << it->base_name << ".o: $(DEPENDS_" << it->base_name << ")" << endl;
		AutoMakefile << "\t$(CC) $(FLAGS) $(INCLUDE) $(EXTERN_INCLUDE) -c " << it->full_name << " -o $(BINDIR)/" << it->base_name << ".o" << endl << endl;
	}

	for(auto it = sources.begin(); it != sources.end(); it++)
	{
		AutoMakefile << "$(BINDIR)/" << it->base_name << ".o: $(DEPENDS_" << it->base_name << ")" << endl;
		AutoMakefile << "\t$(CC) $(FLAGS) $(INCLUDE) $(EXTERN_INCLUDE) -c " << it->full_name << " -o $(BINDIR)/" << it->base_name << ".o" << endl << endl;
	}

	AutoMakefile << "run: $(BINDIR)/" << mains.front().base_name << ".exe" << endl;
	AutoMakefile << "\t" << mains.front().base_name << ".exe" << endl << endl;

	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		AutoMakefile << "run_" << it->base_name << ": $(BINDIR)/" << it->base_name << ".exe" << endl;
		AutoMakefile << "\t" << it->base_name << ".exe" << endl << endl;
	}
	
	AutoMakefile << "clean:" << endl;
	AutoMakefile << "\trm -f $(BINDIR)/*.o" << endl;
	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		AutoMakefile << "\trm -f $(BINDIR)/" << it->base_name << ".exe" << endl;
	}
	AutoMakefile << endl;

	AutoMakefile << "clear:" << endl;
	AutoMakefile << "\trm -f $(BINDIR)/*.o" << endl;

	AutoMakefile.close();

	string dest_dir = "C:/Users/" + username() + "/AppData/Roaming/Sublime Text 3/Packages/User";
	mkdir(dest_dir);
	ofstream sublime_build(dest_dir + "/AutoMake.sublime-build");
	sublime_build << "{" << endl;
	sublime_build << "\t\"encoding\": \"cp936\"," << endl;
	sublime_build << "\t\"selector\": \"source.cpp\"," << endl;
	sublime_build << "\t\"working_dir\": \"$file_path\"," << endl;
	sublime_build << "\t\"file_regex\": \"^((?:.:)?[^:\\n\\r]*):([0-9]+):?([0-9]+)?:? (.*)$\"," << endl;
	sublime_build << "\t\"shell_cmd\": \"automake\"," << endl;
	sublime_build << "\t\"variants\":" << endl;
	sublime_build << "\t[" << endl;
	sublime_build << "\t\t{" << endl;
	sublime_build << "\t\t\t\"name\": \"generate\"," << endl;
	sublime_build << "\t\t\t\"shell_cmd\": \"automake generate\"" << endl;
	sublime_build << "\t\t}," << endl << endl;
	sublime_build << "\t\t{" << endl;
	sublime_build << "\t\t\t\"name\": \"update\"," << endl;
	sublime_build << "\t\t\t\"shell_cmd\": \"automake update\"" << endl;
	sublime_build << "\t\t}," << endl << endl;

	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		sublime_build << "\t\t{" << endl;
		sublime_build << "\t\t\t\"name\": \"" << it->base_name << ".exe\"," << endl;
		sublime_build << "\t\t\t\"shell_cmd\": \"automake " << BINDIR << "/" << it->base_name << ".exe\"" << endl;
		sublime_build << "\t\t}," << endl << endl;
	}

	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		sublime_build << "\t\t{" << endl;
		sublime_build << "\t\t\t\"name\": \"run " << it->base_name << ".exe\"," << endl;
		sublime_build << "\t\t\t\"shell_cmd\": \"automake run_" << it->base_name << "\"" << endl;
		sublime_build << "\t\t}," << endl << endl;
	}

	sublime_build << "\t\t{" << endl;
	sublime_build << "\t\t\t\"name\": \"clear\"," << endl;
	sublime_build << "\t\t\t\"shell_cmd\": \"automake clear\"" << endl;
	sublime_build << "\t\t}," << endl << endl;

	sublime_build << "\t\t{" << endl;
	sublime_build << "\t\t\t\"name\": \"clean\"," << endl;
	sublime_build << "\t\t\t\"shell_cmd\": \"automake clean\"" << endl;
	sublime_build << "\t\t}" << endl;

	sublime_build << "\t]" << endl;
	sublime_build << "}" << endl;
	sublime_build.close();
}

void AutoMaker::generate()
{
	if(!here)
	{
		string init_path = pwd();
		string current_path = init_path;
		Target target(current_path);

		while(target.name[0] != '.' && target.name != "temp" && !is_root(current_path) && !has_makefile(current_path))
		{
			cd("..");
			current_path = pwd();
			target = Target(current_path);
		}

		if(!has_makefile(current_path))
		{
			cd(init_path);
			current_path = init_path;
		}
	}
	get_files();
	extract_info();
	
	long long makefile_data = modify_time("AutoMakefile");
	for(auto it = sources.begin(); it != sources.end();)
	{
		bool is_main = update_depends(*it);
		if(is_main)
		{
			mains.push_back(*it);
			it = sources.erase(it);
			continue;
		}
		it++;
	}

	for(auto it = mains.begin(); it != mains.end();)
	{
		bool is_main = update_depends(*it);
		if(!is_main)
		{
			sources.push_back(*it);
			it = mains.erase(it);
			continue;
		}
		it++;
	}

	write_AutoMakefile();
	mkdir(BINDIR);
}

void AutoMaker::update()
{
	if(!here)
	{
		string init_path = pwd();
		string current_path = init_path;
		Target target(current_path);

		while(target.name[0] != '.' && target.name != "temp" && !is_root(current_path) && !has_makefile(current_path))
		{
			cd("..");
			current_path = pwd();
			target = Target(current_path);
		}

		if(!has_makefile(current_path))
		{
			cd(init_path);
			current_path = init_path;
		}
	}
	get_files();
	extract_info();
	
	long long makefile_data = modify_time("AutoMakefile");
	for(auto it = sources.begin(); it != sources.end();)
	{
		if(!(it->found) || modify_time(it->full_name) > makefile_data)
		{
			bool is_main = update_depends(*it);
			if(is_main)
			{
				mains.push_back(*it);
				it = sources.erase(it);
				continue;
			}
		}
		it++;
	}

	for(auto it = mains.begin(); it != mains.end();)
	{
		if(!(it->found) || modify_time(it->full_name) > makefile_data)
		{
			bool is_main = update_depends(*it);
			if(!is_main)
			{
				sources.push_back(*it);
				it = mains.erase(it);
				continue;
			}
		}
		it++;
	}

	write_AutoMakefile();
	mkdir(BINDIR);
}

void AutoMaker::get_files()
{
	get_files(".");
}

void AutoMaker::get_files(const string& path)
{
    long hFile = 0;  
    struct _finddata_t fileinfo;  
    string p;  
    if((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1)  
    {
        do
        {
            if(fileinfo.attrib & _A_SUBDIR)
            {
            	if(strcmp(fileinfo.name, ".") != 0 &&
            	   strcmp(fileinfo.name, "..") != 0 &&
            	   strcmp(fileinfo.name, "temp") != 0 &&
            	   fileinfo.name[0] != '.')
            	{
            		get_files(p.assign(path).append("/").append(fileinfo.name));
            	}	
            }
            else
            {
            	Target target = Target(p.assign(path).append("/").append(fileinfo.name));
            	if(target.base_name == "temp")
            	{
            		continue;
            	}
            	if(target.expand_name == "c" || target.expand_name == "cpp")
            	{
            		sources.push_back(target);
	            }
	            else if(target.expand_name == "h" || target.expand_name == "hpp" || target.expand_name == "")
	            {
	            	heads.push_back(target);
	            }
	            else if(target.expand_name == "dll" || target.expand_name == "so")
	            {
	            	libs.push_back(target);
	            	RPATH += (target.path + ';');
	            }
	            else if(target.expand_name == "lib" || target.expand_name == "a")
	            {
	            	libs.push_back(target);
	            }
            }
        }
        while(_findnext(hFile, &fileinfo) == 0);

        _findclose(hFile);
    }
}

void AutoMaker::move_to_mains(const string& exes)
{
	int i_name_start = 10;
	int n = exes.size();
	for(int i = i_name_start; i <= n; i++)
	{
		if(i == n || exes[i] == ' ')
		{
			auto it_source = find_name(sources, exes.substr(i_name_start, i-i_name_start-4) + ".cpp");
			if(it_source != sources.end())
			{
				mains.push_back(*it_source);
				sources.erase(it_source);
			}
			i += 10;
			i_name_start = i + 1;
		}
	}
}

void AutoMaker::extract_info()
{
	if(!has_file("AutoMakefile"))
	{
		return;
	}
	
	ifstream AutoMakefile("AutoMakefile");
	string line;

	getline(AutoMakefile, line);
	if(line.substr(0, 5) != "CC = ")
	{
		return;
	}
	CC = line.substr(5, line.size() - 5);

	getline(AutoMakefile, line);
	if(line.substr(0, 8) != "FLAGS = ")
	{
		return;
	}
	FLAGS = line.substr(8, line.size() - 8);

	getline(AutoMakefile, line);
	if(line.substr(0, 13) != "LINK_FLAGS = ")
	{
		return;
	}
	LINK_FLAGS = line.substr(13, line.size() - 13);

	getline(AutoMakefile, line);
	if(line.substr(0, 9) != "BINDIR = ")
	{
		return;
	}
	BINDIR = line.substr(9, line.size() - 9);

	getline(AutoMakefile, line);
	getline(AutoMakefile, line);
	if(line.substr(0, 18) == "EXTERN_INCLUDE = \\")
	{
		getline(AutoMakefile, line);
		while(line != "")
		{
			EXTERN_INCLUDE += (line + '\n');
			getline(AutoMakefile, line);
		}
	}

	getline(AutoMakefile, line);
	if(line.substr(0, 18) == "EXTERN_LIBPATH = \\")
	{
		getline(AutoMakefile, line);
		while(line != "")
		{
			EXTERN_LIBPATH += (line + '\n');
			RPATH += (line.substr(2, line.size()-2) + ';');
			getline(AutoMakefile, line);
		}
	}

	getline(AutoMakefile, line);
	if(line.substr(0, 18) == "EXTERN_LIBS = \\")
	{
		getline(AutoMakefile, line);
		while(line != "")
		{
			EXTERN_LIBS += (line + '\n');
			getline(AutoMakefile, line);
		}
	}
	
	while(true)
	{
		getline(AutoMakefile, line);
		if(line.substr(0, 5) == "all: ")
		{
			move_to_mains(line.substr(5, line.size()-5));
			break;
		}
		if(line.substr(0, 8) == "DEPENDS_")
		{
			getline(AutoMakefile, line);
			string source_name = line.substr(0, line.size() - 2);
			auto it_source = find_name(sources, Target(source_name).name);
			if(it_source == sources.end())
			{
				continue;
			}
			it_source->found = true;
			while(line != "")
			{
				getline(AutoMakefile, line);
				string head_name = line.substr(0, line.size() - 2);
				auto it_head = find_name(heads, Target(head_name).name);
				if(it_head == heads.end())
				{
					continue;
				}
				it_head->found = true;
				it_source->depends.push_back(&(*it_head));
			}
		}
	}
}

bool AutoMaker::update_depends(Target& target)
{
	target.depends.clear();
	ifstream file(target.full_name);
	string line;
	bool is_main = false;
	while(getline(file, line))
	{
		delete_space(line);
		if(line.substr(0, 8) == "intmain(" || line.substr(0, 9) == "voidmain(")
		{
			is_main = true;
		}
		if(line.substr(0, 8) == "#include")
		{
			string head_name = line.substr(9, line.size() - 10);
			auto it = find_name(heads, head_name);
			if(it != heads.end())
			{
				target.depends.push_back(&(*it));
			}
		}
	}

	return is_main;
}

void AutoMaker::make(const string& target)
{
	update();
	system((string("make -f AutoMakefile ") + target).data());
}

void AutoMaker::I(const string& include_path)
{
	EXTERN_INCLUDE += (include_path + " \\\n");
}

void AutoMaker::L(const string& lib_path)
{
	EXTERN_LIBPATH += (lib_path + " \\\n");
	RPATH += (lib_path.substr(2, lib_path.size()-2) + ";");
}

void AutoMaker::l(const string& lib_name)
{
	EXTERN_LIBS += (lib_name + " \\\n");
}

void AutoMaker::add_flag(const string& flag)
{
	FLAGS += (string(" ") + flag);
}

void AutoMaker::set_BinDir(const string& bindir)
{
	BINDIR = bindir;
}