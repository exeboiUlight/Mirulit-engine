@echo off
windres app.rc -o app.o
editor.exe config.c include/glad/glad.c -l kernel32 -l user32 -l gdi32 -l opengl32
pause