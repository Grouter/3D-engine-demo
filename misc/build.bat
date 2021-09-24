@echo off

if not exist w:\build mkdir w:\build
pushd w:\build

cl -Zi w:\src\main.cpp user32.lib gdi32.lib opengl32.lib

popd