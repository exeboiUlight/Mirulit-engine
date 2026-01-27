@echo off
windres app.rc -o app.o
editor.exe config.c -l opengl32 -l gdi32 -l SDL3 -o bin/game.exe
pause