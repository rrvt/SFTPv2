
set WorkSpace=%~1
IF %WorkSpace:~-1%==\ SET WorkSpace=%WorkSpace:~0,-1%
set WorkSpace=%WorkSpace%..\

cd %WorkSpace%

rem Setup Compiler system variables -- required for nmake to run

set vs2022="C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars32.bat"
CALL %vs2022%

nmake
nmake test
nmake install
exit

