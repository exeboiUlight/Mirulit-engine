import os

os.system("gcc main.c include/glad/glad.c -Iinclude -Icore -Llib -llua54 -lglfw3 -lopengl32 -lgdi32 -lwinmm -lm -o bin/Mirulit.exe")