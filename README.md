# AutoMake - Automatically build your C++ project

## Why AutoMake?
If you are using IDE, you cannot change anything without your IDE: you can't move a cpp file to another folder, you cannot new a head file in a folder you choose, you can't copy a library into your project folder then directly using it.

Why you can't? Because all these operations are not doing with IDE, so IDE will not change the project description file, then IDE will not know where to find you .cpp .h .dll .lib.

If you want to change your project directory, such as create, move, rename some files or folders, add some external librarys, you need to wait for 5 minites to open your Visual Studio, then click a mess of buttons, change include path, library path, runtime path... It's stupid, isn't it? IDE is a shackles, is a cage.

If you don't use IDE, you cannot avoid writing Makefile by your self. It's also difficult to maintain the Makefile when changing directory structure.

Don't worry, AutoMake will help you to deal with these. No matter how complex your project directory is, AutoMake will generate a correct Makefile and call ``make`` command. You can change your directory structure freely, you can put your source files, head files, static library files, dynamic library files any where and create them, delete them, move them, rename them anytime you want. AutoMake tool will find the dependent relationship between them and just generate a correct Makefile.

## Installing
I need to declare here, this tool is only suitable for GCC build system users under Windows. So if you are not using GCC or not using Windows, you cannot use my tool. Don't worry, other platforms' support will be developed in newer versions.

So how to get the tool and use it? Download this project and execute command ``make`` to build it. Then automake.exe will be generated. Copy it to where ever you want. Then add the path of this exe to System Environment Variable Path(if you want).

If you are a Sublime Text user and you want to use this tool by Ctrl + B, just execute command ``make install``. Then everything will be done.(To uninstall just use command ``make uninstall``). After that, you can change the build system to AutoMake, Ctrl + Shift + B to choose which option to use.

## Have a try
Go to .examples folder. Call command ``automake here run``. It will run as expect.
Let's see what's happening. Here is a little project with complex structure, firstly the directory tree is like following:

.examples  
├─ add  
│　├─ include  
│　│　└─ add.h  
│　└─ src  
│　　　└─ add.cpp  
├─ sub  
│　├─ include  
│　│　└─ sub.h  
│　└─ lib  
│　　　└─ libsub.dll  
└─ main.cpp

``main.cpp`` includes ``add.h`` and ``sub.h``, ``add.cpp`` include ``add.h``, when linking, you need link to ``libsub.dll``, when runing, you need ``./sub/lib`` in your PATH. All of these dependents will be found out by AutoMake. You can see an AutoMakefile is generated under ``.example`` folder. You can see the content in AutoMakefile if you want.

## Usage

### Command Parameters
The command form is
```
automake [options] [target]
```

The ``[options]`` can be:
* ``<with-no-arguments>``: Build your project and generate executable file.
* ``here``: If you don't use ``here`` argument, ``automake`` will executes ``cd ..`` continuously util it find a Makefile or AutoMakefile. Then ``automake`` will regard there as the root path of project and do thing there. But if you use ``here`` argument, ``automake`` will take current directory as the root path of project.
* ``DESTDIR=<path>`` or ``BINDIR=<path>``: Indicate where to put the object files and executable files. Default DESTDIR is "./"
* ``update`` or ``generate``: Only generate Makefile(named with "AutoMakefile") but don't call ``make`` command.
* ``-I<path>``: Add external include path for project. Using it only when the include path is out of your project folder. Because all include path inside the project folder can be found by ``automake`` automatically.
* ``-L<path>``: Add external library path for project.
* ``-l<lib-name>``: Add external library file for project.
* ``<others>``: All other parameters will be seen as compiler flags and passed to the compiler. That means ``gcc`` and ``g++`` flags.

The ``[target]`` can be:
* ``run``: Build your project, generate executable file and execute it.
* ``clean``: Clean all object files and executable files

Here I need to explain something: Why if you don't use ``here`` option, ``automake`` will go upper upper and upper? Because when you are writing code at a deep directory of your project, you will not want to go back to project root folder to call ``automake``. You just want to call ``automake`` here. So, I make it possible. If you are using Sublime Text, you can press Ctrl + B on any file and don't need to worry about here dosen't has a Makefile, just press Ctrl + B.

### Change options from file
You can use above parameters to set automake options. You can also change them from "AutoMakefile". Execute
``automake generate`` to generate an "AutoMakefile", open it, And you can change these fields:
```
CC = <specify compiler> 
FLAGS = <specify compiler flags>
LINK_FLAGS = <specify link flags>
BINDIR = <dir to store .o and .exe>

EXTERN_INCLUDE = \
-I<path1> \
-I<path2> \

EXTERN_LIBPATH = \
-L<path3> \
-L<path4> \

EXTERN_LIBS = \
-l<lib1> \
-l<lib2> \
```
Attention:  
Don't bring the angle brace "<>" when you truly change this file.

After you change, ``automake`` will remember your options and next time you call ``automake`` it will use these options.

### Comment out your files
If you don't want ``automake`` to compile some specific files, put them in a folder named "temp", or named begin with ".". You can also chang the file base name into "temp" to let ``automake`` ignore this file.

Go to your project folder and automake it!