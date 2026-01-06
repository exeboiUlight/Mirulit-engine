@echo off

gcc editor.c include/glad/glad.c -o dist/editor.exe -L./ -L./dist -llibtcc -lglfw3 -lgdi32 -limm32 -lole32 -lcomctl32 -lopengl32 -luuid -Iinclude -Iexternal