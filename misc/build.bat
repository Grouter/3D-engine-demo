@echo off

set BuildCompilerFlags=-Zi
set BuildLinkerFlags=user32.lib gdi32.lib opengl32.lib

if not exist w:\build mkdir w:\build
pushd w:\build

cl %BuildCompilerFlags% w:\src\main.cpp %BuildLinkerFlags%

popd