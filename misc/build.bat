@echo off

:: Look here for compiler flags documentation https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically?view=msvc-160
set BuildCompilerFlags=-DGLEW_STATIC -MT -nologo -Gm- -GR- -EHa- -Oi -W4 -wd4201 -wd4100 -Zi -I ../libs/
:: Link libraries here
set BuildLinkerFlags=-opt:ref user32.lib gdi32.lib ../libs/glew32s.lib opengl32.lib

pushd %~dp0%
cd ..
if not exist build mkdir build
cd build

:: 32 bit build
:: cl %BuildCompilerFlags% ..\src\main.cpp /link -subsystem:windows,5.01 %BuildLinkerFlags%

cl %BuildCompilerFlags% ..\src\main.cpp /link %BuildLinkerFlags%

popd
