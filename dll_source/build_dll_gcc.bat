@echo off
if exist %~dp0*.o del %~dp0*.o
gcc -c %~dp0*lib.c -O2  -std=c11 -Wall  -Wno-missing-braces -D CALCDLL_EXPORTS
gcc -o %~dp0nlib_gcc.dll %~dp0*lib.o  -s -shared -W -mwindows -I ./include/ -L ./lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
if exist %~dp0*.o del %~dp0*.o