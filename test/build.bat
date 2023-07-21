@echo off

cl /EHsc test.cpp

del -f test.obj

if exist test.exe goto execute
goto end

:execute
    test.exe

:end
