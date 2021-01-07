@echo off
rem Move into directory of this script.
set script_dir=%~dp0
set script_dir=%script_dir:~0, -1%
cd %script_dir%

rem Create solution
cmake -G "Visual Studio 16 2019" -A x64 -S ..\ -B .
echo Press any key to exit.
pause >nul
