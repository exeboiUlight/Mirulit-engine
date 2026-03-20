import os

os.system("g++ src/main.cpp include/glad/glad.c -lglfw3 -lopengl32 -lgdi32 -I./include -L./lib -o ./bin")