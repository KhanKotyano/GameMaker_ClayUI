@echo off
if exist %~dp0*.o del %~dp0*.o
clang -c %~dp0*lib.c -O2  -std=c11  -Wall -Wno-missing-braces -D CALCDLL_EXPORTS
clang -o %~dp0nlib.dll %~dp0*lib.o  -s -shared -Wl -I ./include/ -L ./lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
if exist %~dp0*.o del %~dp0*.o