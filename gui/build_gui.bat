@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
cd /d "D:\vvtts\src\gui"
echo Compiling Resources...
rc /nologo gui.rc
echo Compiling Application...
cl /nologo /O2 /W3 /D WIN32 /D _WINDOWS /D _CRT_SECURE_NO_WARNINGS main.cpp gui.res user32.lib gdi32.lib comdlg32.lib /link /SUBSYSTEM:WINDOWS /OUT:..\..\VVTTS_GUI.exe
if %ERRORLEVEL% equ 0 (
    echo Build successful. Output is at D:\vvtts\VVTTS_GUI.exe
) else (
    echo Build failed.
)
