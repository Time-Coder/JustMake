#include "justmake.h"

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

bool has_makefile(const string& path)
{
	return dir::is_file("Makefile") || dir::is_file("makefile") || dir::is_file(".Makefile");
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

long modify_time(const string& filename)
{
	if(!exist(filename))
	{
		return 0;
	}

	FILE *fp = fopen(filename.c_str(), "r");
	int fd = fileno(fp);
	struct stat buf;
	fstat(fd, &buf);
	fclose(fp);
	
	return buf.st_mtime;
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

void JustMaker::write_Makefile()
{
	ofstream Makefile(".Makefile");

	Makefile << "CC = " << CC << endl;
	Makefile << "FLAGS = " << FLAGS << endl;
	Makefile << "LINK_FLAGS = " << LINK_FLAGS << endl;
	Makefile << "BINDIR = " << BINDIR << endl << endl;
	Makefile << "EXEDIR = " << EXEDIR << endl << endl;

	Makefile << "EXTERN_INCLUDE = \\\n" << EXTERN_INCLUDE << endl;
	Makefile << "EXTERN_LIBPATH = \\\n" << EXTERN_LIBPATH << endl;
	Makefile << "EXTERN_LIBS = \\\n" << EXTERN_LIBS << endl;

	string last_path = "";
	Makefile << "INCLUDE = \\" << endl;
	for(auto it = heads.begin(); it != heads.end(); it++)
	{
		if(it->path != last_path)
		{
			Makefile << "-I" << it->path << " \\" << endl;
		}
		last_path = it->path;
	}
	Makefile << endl;

	last_path = "";
	Makefile << "LIBPATH = \\" << endl;
	for(auto it = libs.begin(); it != libs.end(); it++)
	{
		if(it->path != last_path)
		{
			Makefile << "-L" << it->path << " \\" << endl;
		}
		last_path = it->path;
	}
	Makefile << endl;

	Makefile << "LIBS = \\" << endl;
	for(auto it = libs.begin(); it != libs.end(); it++)
	{
		Makefile << "-l" << it->lib_base << " \\" << endl;
	}
	Makefile << endl;

	Makefile << "OBJS = \\" << endl;
	for(auto it = sources.begin(); it != sources.end(); it++)
	{
		Makefile << "$(BINDIR)/" << it->base_name << ".o \\" << endl;
	}
	Makefile << endl;

	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		Makefile << "DEPENDS_" << it->base_name << " = \\" << endl
					 << it->full_name << " \\" << endl;
		for(auto it_depends = it->depends.begin(); it_depends != it->depends.end(); it_depends++)
		{
			Makefile << (*it_depends)->full_name << " \\" << endl;
		}
		Makefile << endl;
	}

	for(auto it = sources.begin(); it != sources.end(); it++)
	{
		Makefile << "DEPENDS_" << it->base_name << " = \\" << endl
					 << it->full_name << " \\" << endl;
		for(auto it_depends = it->depends.begin(); it_depends != it->depends.end(); it_depends++)
		{
			Makefile << (*it_depends)->full_name << " \\" << endl;
		}
		Makefile << endl;
	}

	if(!RPATH.empty())
	{
		Makefile << "PATH := " << RPATH << "$(PATH)" << endl << endl;
	}

	Makefile << "all: ";
	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		Makefile << "$(EXEDIR)/" << it->base_name << ".exe ";
	}
	Makefile << endl << endl;

	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		Makefile << "$(EXEDIR)/" << it->base_name << ".exe: $(BINDIR)/" << it->base_name << ".o $(OBJS)" << endl;
		Makefile << "\t$(CC) $(LINK_FLAGS) $(BINDIR)/" << it->base_name << ".o $(OBJS) -o $(EXEDIR)/" << it->base_name << ".exe" << " $(LIBPATH) $(EXTERN_LIBPATH) $(LIBS) $(EXTERN_LIBS)" << endl << endl;
	}

	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		Makefile << "$(BINDIR)/" << it->base_name << ".o: $(DEPENDS_" << it->base_name << ")" << endl;
		Makefile << "\t$(CC) $(FLAGS) $(INCLUDE) $(EXTERN_INCLUDE) -c " << it->full_name << " -o $(BINDIR)/" << it->base_name << ".o" << endl << endl;
	}

	for(auto it = sources.begin(); it != sources.end(); it++)
	{
		Makefile << "$(BINDIR)/" << it->base_name << ".o: $(DEPENDS_" << it->base_name << ")" << endl;
		Makefile << "\t$(CC) $(FLAGS) $(INCLUDE) $(EXTERN_INCLUDE) -c " << it->full_name << " -o $(BINDIR)/" << it->base_name << ".o" << endl << endl;
	}

	Makefile << "run: $(EXEDIR)/" << mains.front().base_name << ".exe" << endl;
	Makefile << "\t" << "$(EXEDIR)/" << mains.front().base_name << ".exe" << endl << endl;

	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		Makefile << "run_" << it->base_name << ": $(EXEDIR)/" << it->base_name << ".exe" << endl;
		Makefile << "\t" << "$(EXEDIR)/" << it->base_name << ".exe" << endl << endl;
	}
	
	Makefile << "clean:" << endl;
	Makefile << "\trm -f $(BINDIR)/*.o" << endl;
	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		Makefile << "\trm -f $(EXEDIR)/" << it->base_name << ".exe" << endl;
	}
	Makefile << endl;

	Makefile << "clear:" << endl;
	Makefile << "\trm -f $(BINDIR)/*.o" << endl;

	Makefile.close();

	#ifdef __linux__
		string dest_dir = "/home/" + username() + "/.config/sublime-text-3/Packages/User";
	#else
		string dest_dir = "C:/Users/" + username() + "/AppData/Roaming/Sublime Text 3/Packages/User";
	#endif

	mkdir(dest_dir);
	ofstream sublime_build(dest_dir + "/JustMake.sublime-build");
	sublime_build << "{" << endl;
	#ifdef __linux__
		sublime_build << "\t\"encoding\": \"utf-8\"," << endl;
	#else
		sublime_build << "\t\"encoding\": \"cp936\"," << endl;
	#endif
	sublime_build << "\t\"selector\": \"source.c.cpp\"," << endl;
	sublime_build << "\t\"working_dir\": \"$file_path\"," << endl;
	sublime_build << "\t\"file_regex\": \"^((?:.:)?[^:\\n\\r]*):([0-9]+):?([0-9]+)?:? (.*)$\"," << endl;
	sublime_build << "\t\"shell_cmd\": \"justmake\"," << endl;
	sublime_build << "\t\"variants\":" << endl;
	sublime_build << "\t[" << endl;
	sublime_build << "\t\t{" << endl;
	sublime_build << "\t\t\t\"name\": \"generate\"," << endl;
	sublime_build << "\t\t\t\"shell_cmd\": \"justmake generate\"" << endl;
	sublime_build << "\t\t}," << endl << endl;
	sublime_build << "\t\t{" << endl;
	sublime_build << "\t\t\t\"name\": \"update\"," << endl;
	sublime_build << "\t\t\t\"shell_cmd\": \"justmake update\"" << endl;
	sublime_build << "\t\t}," << endl << endl;

	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		sublime_build << "\t\t{" << endl;
		sublime_build << "\t\t\t\"name\": \"" << it->base_name << ".exe\"," << endl;
		sublime_build << "\t\t\t\"shell_cmd\": \"justmake " << EXEDIR << "/" << it->base_name << ".exe\"" << endl;
		sublime_build << "\t\t}," << endl << endl;
	}

	for(auto it = mains.begin(); it != mains.end(); it++)
	{
		sublime_build << "\t\t{" << endl;
		sublime_build << "\t\t\t\"name\": \"run " << it->base_name << ".exe\"," << endl;
		sublime_build << "\t\t\t\"shell_cmd\": \"justmake run_" << it->base_name << "\"" << endl;
		sublime_build << "\t\t}," << endl << endl;
	}

	sublime_build << "\t\t{" << endl;
	sublime_build << "\t\t\t\"name\": \"clear\"," << endl;
	sublime_build << "\t\t\t\"shell_cmd\": \"justmake clear\"" << endl;
	sublime_build << "\t\t}," << endl << endl;

	sublime_build << "\t\t{" << endl;
	sublime_build << "\t\t\t\"name\": \"clean\"," << endl;
	sublime_build << "\t\t\t\"shell_cmd\": \"justmake clean\"" << endl;
	sublime_build << "\t\t}" << endl;

	sublime_build << "\t]" << endl;
	sublime_build << "}" << endl;
	sublime_build.close();
}

void JustMaker::generate()
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
	read_Makefile();
	
	long makefile_data = modify_time(".Makefile");
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

	write_Makefile();
	mkdir(BINDIR);
}

void JustMaker::update()
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
	read_Makefile();
	

	long makefile_data = modify_time(".Makefile");

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

	write_Makefile();
	mkdir(BINDIR);
}

void JustMaker::get_files(string dirname)
{
	dir::format(dirname);
	if(dirname[dirname.size()-1] == '/')
	{
		dirname.erase(dirname.size()-1, 1);
	}

    struct stat s;
    stat(dirname.c_str(), &s);
    if(!S_ISDIR(s.st_mode))
    {
        return;
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
        string name = dirname + string("/") + string(p->d_name);
        format(name);
        stat(name.c_str(), &st);
        if(S_ISDIR(st.st_mode))
        {
        	if(p->d_name[0] != '.' && strcmp(p->d_name, "temp") != 0)
        	{
            	get_files(name);
            }
        }
        else
        {
            Target target = Target(name);
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
            	#ifdef __linux__
            		RPATH += (target.path + ':');
            	#else
            		RPATH += (target.path + ';');
            	#endif
            }
            else if(target.expand_name == "lib" || target.expand_name == "a")
            {
            	libs.push_back(target);
            }
        }
    }
    closedir(open_dir);
}

// void JustMaker::get_files(const string& path)
// {
//     long hFile = 0;
//     struct _finddata_t fileinfo;
//     string p;
//     if((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1)
//     {
//         do
//         {
//             if(fileinfo.attrib & _A_SUBDIR)
//             {
//             	if(strcmp(fileinfo.name, ".") != 0 &&
//             	   strcmp(fileinfo.name, "..") != 0 &&
//             	   strcmp(fileinfo.name, "temp") != 0 &&
//             	   fileinfo.name[0] != '.')
//             	{
//             		get_files(p.assign(path).append("/").append(fileinfo.name));
//             	}
//             }
//             else
//             {
//             	Target target = Target(p.assign(path).append("/").append(fileinfo.name));
//             	if(target.base_name == "temp")
//             	{
//             		continue;
//             	}
//             	if(target.expand_name == "c" || target.expand_name == "cpp")
//             	{
//             		sources.push_back(target);
// 	            }
// 	            else if(target.expand_name == "h" || target.expand_name == "hpp" || target.expand_name == "")
// 	            {
// 	            	heads.push_back(target);
// 	            }
// 	            else if(target.expand_name == "dll" || target.expand_name == "so")
// 	            {
// 	            	libs.push_back(target);
// 	            	RPATH += (target.path + ';');
// 	            }
// 	            else if(target.expand_name == "lib" || target.expand_name == "a")
// 	            {
// 	            	libs.push_back(target);
// 	            }
//             }
//         }
//         while(_findnext(hFile, &fileinfo) == 0);

//         _findclose(hFile);
//     }
// }

void JustMaker::move_to_mains(const string& exes)
{
	int i_name_start = 10;
	int n = exes.size();
	for(int i = i_name_start; i <= n; i++)
	{
		if(i == n || exes[i] == ' ')
		{
			auto it_source = find_name(sources, exes.substr(i_name_start, i-i_name_start-4) + ".cpp");
			if(it_source == sources.end())
			{
				it_source = find_name(sources, exes.substr(i_name_start, i-i_name_start-4) + ".c");
			}
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

void JustMaker::read_Makefile()
{
	if(!dir::is_file(".Makefile"))
	{
		return;
	}
	
	ifstream Makefile(".Makefile");
	string line;

	getline(Makefile, line);
	if(line.substr(0, 5) != "CC = ")
	{
		return;
	}
	CC = line.substr(5, line.size() - 5);

	getline(Makefile, line);
	if(line.substr(0, 8) != "FLAGS = ")
	{
		return;
	}
	FLAGS = line.substr(8, line.size() - 8);

	getline(Makefile, line);
	if(line.substr(0, 13) != "LINK_FLAGS = ")
	{
		return;
	}
	LINK_FLAGS = line.substr(13, line.size() - 13);

	getline(Makefile, line);
	if(line.substr(0, 9) != "BINDIR = ")
	{
		return;
	}
	BINDIR = line.substr(9, line.size() - 9);

	getline(Makefile, line);
	if(line.substr(0, 9) != "EXEDIR = ")
	{
		return;
	}
	EXEDIR = line.substr(9, line.size() - 9);

	getline(Makefile, line);
	getline(Makefile, line);
	if(line.substr(0, 18) == "EXTERN_INCLUDE = \\")
	{
		getline(Makefile, line);
		while(line != "")
		{
			EXTERN_INCLUDE += (line + '\n');
			getline(Makefile, line);
		}
	}

	getline(Makefile, line);
	if(line.substr(0, 18) == "EXTERN_LIBPATH = \\")
	{
		getline(Makefile, line);
		while(line != "")
		{
			EXTERN_LIBPATH += (line + '\n');
			RPATH += (line.substr(2, line.size()-2) + ';');
			getline(Makefile, line);
		}
	}

	getline(Makefile, line);
	if(line.substr(0, 18) == "EXTERN_LIBS = \\")
	{
		getline(Makefile, line);
		while(line != "")
		{
			EXTERN_LIBS += (line + '\n');
			getline(Makefile, line);
		}
	}
	
	while(true)
	{
		getline(Makefile, line);
		if(line.substr(0, 5) == "all: ")
		{
			move_to_mains(line.substr(5, line.size()-5));
			break;
		}
		if(line.substr(0, 8) == "DEPENDS_")
		{
			getline(Makefile, line);
			string source_name = line.substr(0, line.size() - 2);
			auto it_source = find_name(sources, Target(source_name).name);
			if(it_source == sources.end())
			{
				continue;
			}
			it_source->found = true;
			while(line != "")
			{
				getline(Makefile, line);
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

bool JustMaker::update_depends(Target& target)
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

void JustMaker::make(const string& target)
{
	update();
	system((string("make -f .Makefile ") + target).data());
}

void JustMaker::I(const string& include_path)
{
	EXTERN_INCLUDE += (include_path + " \\\n");
}

void JustMaker::L(const string& lib_path)
{
	EXTERN_LIBPATH += (lib_path + " \\\n");
	RPATH += (lib_path.substr(2, lib_path.size()-2) + ";");
}

void JustMaker::l(const string& lib_name)
{
	EXTERN_LIBS += (lib_name + " \\\n");
}

void JustMaker::add_flag(const string& flag)
{
	FLAGS += (string(" ") + flag);
}

void JustMaker::set_BinDir(const string& bindir)
{
	BINDIR = bindir;
}