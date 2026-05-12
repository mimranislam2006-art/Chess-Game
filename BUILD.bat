@echo off
echo ============================================
echo  Chess Game - Build Script
echo ============================================

call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64

set CMAKE="C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set NINJA="C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
set SRC=C:\Users\HB LAPTOP\Downloads\Chess\Chess\ChessGame
set BUILD=C:\Users\HB LAPTOP\Downloads\Chess\Chess\ChessGame\out\build\ManualBuild
set SFML=C:/Users/HB LAPTOP/Downloads/SFML-3.0.0/lib/cmake/SFML

echo Configuring...
%CMAKE% -G Ninja -DCMAKE_BUILD_TYPE=Release -DSFML_DIR="%SFML%" -DCMAKE_MAKE_PROGRAM=%NINJA% -S "%SRC%" -B "%BUILD%"

echo Building...
%CMAKE% --build "%BUILD%"

echo Copying SFML DLLs...
copy /Y "C:\Users\HB LAPTOP\Downloads\SFML-3.0.0\bin\sfml-graphics-3.dll" "%BUILD%\"
copy /Y "C:\Users\HB LAPTOP\Downloads\SFML-3.0.0\bin\sfml-window-3.dll"   "%BUILD%\"
copy /Y "C:\Users\HB LAPTOP\Downloads\SFML-3.0.0\bin\sfml-system-3.dll"   "%BUILD%\"

echo.
echo ============================================
echo  Build complete! Launching game...
echo ============================================
cd /d "%BUILD%"
start "" "%BUILD%\ChessGame.exe"