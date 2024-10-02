rem first argument is the configuration (i.e. Debug or Release)
rem second argument is path to openSSL directory base (e.g. D:\Sources\OpenSSL.3.1\)
rem third argument is "static" to indicate a static library (rather than a dll library)

rem   Build Solution Line:  tcc  WinBuild %bn "%rw" static
rem   Be sure that %rw contains no spaces (too much appending is done that does not deal with quote marks
rem   Add Build Solution line to

echo on

set Configuration=%1
set WorkSpace=%~2
IF %WorkSpace:~-1%==\ SET WorkSpace=%WorkSpace:~0,-1%
set WorkSpace=%WorkSpace%..\

set OpenSSLsrc=%WorkSpace%

set OpenSSLtgt=%WorkSpace%%Configuration%\

set Dirs=--prefix=%OpenSSLtgt% --openssldir=%OpenSSLtgt% --libdir=..\%Configuration%

iff "%Configuration%" == "Debug" then
  set flavor=--debug
else
  set flavor=--release
endiff

iff "%3" == "static" then
  set Static=no-shared
else
  set Static=
endiff

cd %OpenSSLsrc%

rem Setup Compiler system variables -- required for nmake to run

set vs2022="C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars32.bat"
CALL %vs2022%

rem Create makefile

md /d/Ne %OpenSSLsrc%

perl Configure VC-WIN32 %flavor%  %Dirs% %Static% no-capieng


nmake
nmake test
nmake install
exit

