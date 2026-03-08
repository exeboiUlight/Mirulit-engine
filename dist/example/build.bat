@echo off
windres app.rc -o app.o
compiler.exe config.c -l opengl32 -l mirulit -l gdi32 -o bin/game.exe
pause