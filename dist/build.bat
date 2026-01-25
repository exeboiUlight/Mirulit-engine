@echo off
windres app.rc -o app.o
editor.exe config.c include/glad/glad.c -l glfw3 -l opengl32 -l gdi32 -o bin/game.exe
pause