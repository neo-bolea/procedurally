@echo off
robocopy "P:\C++\Procedurally\src\include" "P:\C++\Procedurally\build\include" /E /NFL /NDL /NJH /NJS /PURGE *.h *.inc
EXIT 0