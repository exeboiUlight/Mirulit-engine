import os

os.system("g++ src/main.cpp include/glad/glad.c -lglfw3 -lopengl32 -lgdi32 -lfreetype -I./include -L./lib -o ./bin/Nirulit")