@echo off
%~d0:
cd %~dp0\BIN
oflash 0 1 1 0 0 AllModuleTestsofJZ2440.bin
@echo on
