@echo off
windres app.rc -o app.o
editor.exe -I ./engine/include config.c engine/include/mirulit/window.c -l kernel32 -l user32 -l gdi32 -l opengl32 -l winmm
pause