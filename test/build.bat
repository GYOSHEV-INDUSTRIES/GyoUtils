@echo off

cl /nologo /W0 /O2 /EHsc test.cpp /link /OUT:test.exe

del -f test.obj

if exist test.exe goto execute
goto end

:execute
    test.exe

:end
