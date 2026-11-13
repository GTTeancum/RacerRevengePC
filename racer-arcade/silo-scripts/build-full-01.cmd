@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
set TEMP=D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\temp
set TMP=D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\temp
powershell -NoProfile -ExecutionPolicy Bypass -File D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\tree\platform\windows\native\Build-Interactive50DU.ps1 -SourceRoot D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\tree -BuildDirectory D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\tree\build\silo-full-01 -Compiler "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\cl.exe" -Linker "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\link.exe" -SDL2IncludeDirectory D:\Programming\GitHub\RacerRevengePC\toolchains\sdl2-msvc\extracted\SDL2-2.32.10\include -SDL2RuntimeDll D:\Programming\GitHub\RacerRevengePC\toolchains\sdl2-msvc\extracted\SDL2-2.32.10\lib\x64\SDL2.dll -SourceRevision silo-865b3a9 -MaxCompileJobs 4
echo BUILD_EXIT %ERRORLEVEL%
