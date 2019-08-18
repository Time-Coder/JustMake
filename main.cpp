#include "automake.h"

int main(int argc, char** argv)
{
	AutoMaker maker;
	string target = "";
	bool dont_make = false;
	for(int i = 1; i < argc; i++)
	{
		string arg = string(argv[i]);
		if(arg == "update" || arg == "generate")
		{
			dont_make = true;
			continue;
		}

		if(arg == "here")
		{
			maker.here = true;
		}

		if(arg.substr(0, 8) == "DESTDIR=")
		{
			maker.set_BinDir(arg.substr(8, arg.size()-8));
			continue;
		}
		if(arg.substr(0, 7) == "BINDIR=")
		{
			maker.set_BinDir(arg.substr(7, arg.size()-7));
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

	if(dont_make)
	{
		maker.update();
	}
	else
	{
		maker.make(target);
	}

	return 0;
}