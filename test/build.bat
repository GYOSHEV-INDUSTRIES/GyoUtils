@echo off

set debug=           -FC                 &:: produce full path of the source code file
set debug=%debug%    -Z7                 &:: produce debug information

cl /nologo /W0 /Od /EHsc %debug% -Ftest.map test.cpp /link /OUT:test.exe
if %errorlevel% neq 0 goto end
del -f test.obj

if exist test.exe goto execute
goto end

:execute
    test.exe

:end
