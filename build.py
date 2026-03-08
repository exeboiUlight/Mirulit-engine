import os
import sys

if sys.platform == "win32":
    # os.system("gcc editor.c app.o -L./ -llibtcc -o dist/example/compiler.exe")
    # os.system("gcc editor_gui.c include/glad/glad.c -o dist/editor.exe app.o -Iinclude -L./ -lglfw3dll -lopengl32 -lgdi32 -static")
    os.system("gcc -shared __mirulit.c include/glad/glad.c -o dist/example/lib/mirulit.dll app.o -Iinclude -L./ -lglfw3dll -lopengl32 -lgdi32 -static")
    os.system("gcc -shared __mirulit.c include/glad/glad.c -o dist/example/bin/mirulit.dll app.o -Iinclude -L./ -lglfw3dll -lopengl32 -lgdi32 -static")