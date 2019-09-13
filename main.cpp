#include "justmake.h"

int main(int argc, char** argv)
{
	JustMaker maker;
	string target = "";
	bool generate = false;
	bool update = false;
	bool clean = false;
	bool clear = false;
	for(int i = 1; i < argc; i++)
	{
		string arg = string(argv[i]);
		if(arg == "update")
		{
			update = true;
			continue;
		}

		if(arg == "generate")
		{
			generate = true;
			continue;
		}

		if(arg == "here")
		{
			maker.here = true;
			continue;
		}

		if(arg == "clean")
		{
			clean = true;
			continue;
		}

		if(arg == "clear")
		{
			clear = true;
			continue;
		}

		if(arg.substr(0, 8) == "EXEDIR=")
		{
			maker.set_EXEDIR(arg.substr(8, arg.size()-8));
			continue;
		}
		if(arg.substr(0, 7) == "BINDIR=")
		{
			maker.set_BINDIR(arg.substr(7, arg.size()-7));
			continue;
		}

		if(arg[0] == '-')
		{
			switch(arg[1])
			{
				case 'I': maker.I(arg); break;
				case 'L': maker.L(arg); break;
				case 'l': maker.l(arg); break;
				default: maker.add_flag(arg);
			}
		}
		else
		{
			target = arg;
		}
	}

	#ifndef __linux__
		if(clean)
		{
			maker.clean();
			return 0;
		}

		if(clear)
		{
			maker.clear();
			return 0;
		}
	#endif

	if(generate)
	{
		maker.generate();
		return 0;
	}

	if(update)
	{
		maker.update();
		return 0;
	}
	maker.make(target);

	return 0;
}