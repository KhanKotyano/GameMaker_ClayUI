@echo off
if exist %~dp0*.o del %~dp0*.o
clang -c %~dp0*lib.c -O2  -std=c11 -Wall  -Wno-missing-braces -D CALCDLL_EXPORTS
clang -o %~dp0ClayUI.dll %~dp0*lib.o  -s -shared -Wl
if exist %~dp0*.o del %~dp0*.o