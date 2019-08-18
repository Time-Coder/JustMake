DESTDIR = D:/AutoMake
SUBLIME = C:\Users\$(USERNAME)\AppData\Roaming\Sublime Text 3\Packages\User

all: automake.exe

automake.exe: main.o automake.o
	g++ *.o -o automake.exe

main.o: main.cpp automake.h
	g++ -std=c++11 -c main.cpp

automake.o: automake.cpp automake.h
	g++ -std=c++11 -c automake.cpp

clean:
	if exist *.o (del *.o)
	if exist automake.exe (del automake.exe)
	if exist AutoMakefile (del AutoMakefile)

install: automake.exe
	@echo {>scripts/AutoMake.sublime-build
	@echo     "encoding": "cp936",>>scripts/AutoMake.sublime-build
	@echo     "working_dir": "$$file_path",>>scripts/AutoMake.sublime-build
	@echo     "file_regex": "^((?:.:)?[^:\\n\\r]*):([0-9]+):?([0-9]+)?:? (.*)$$",>>scripts/AutoMake.sublime-build
	@echo     "shell_cmd": "$(DESTDIR)/automake",>>scripts/AutoMake.sublime-build
	@echo     "variants":>>scripts/AutoMake.sublime-build
	@echo     [>>scripts/AutoMake.sublime-build
	@echo         {>>scripts/AutoMake.sublime-build
	@echo             "name": "run",>>scripts/AutoMake.sublime-build
	@echo             "shell_cmd": "$(DESTDIR)/automake run">>scripts/AutoMake.sublime-build
	@echo         },>>scripts/AutoMake.sublime-build
	@echo.>>scripts/AutoMake.sublime-build
	@echo         {>>scripts/AutoMake.sublime-build
	@echo             "name": "clean",>>scripts/AutoMake.sublime-build
	@echo             "shell_cmd": "$(DESTDIR)/automake clean",>>scripts/AutoMake.sublime-build
	@echo         },>>scripts/AutoMake.sublime-build
	@echo.>>scripts/AutoMake.sublime-build
	@echo         {>>scripts/AutoMake.sublime-build
	@echo             "name": "generate",>>scripts/AutoMake.sublime-build
	@echo             "shell_cmd": "$(DESTDIR)/automake generate">>scripts/AutoMake.sublime-build
	@echo         }>>scripts/AutoMake.sublime-build
	@echo     ]>>scripts/AutoMake.sublime-build
	@echo }>>scripts/AutoMake.sublime-build
	if not exist "$(DESTDIR)" (mkdir "$(DESTDIR)")
	copy automake.exe "$(DESTDIR)" > nul
	copy scripts/AutoMake.sublime-build "$(SUBLIME)" > nul

uninstall:
	if exist "$(DESTDIR)" (del /q "$(DESTDIR)")
	if exist "$(SUBLIME)\AutoMake.sublime-build" (del "$(SUBLIME)\AutoMake.sublime-build")