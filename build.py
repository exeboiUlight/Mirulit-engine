import os
import sys

if sys.platform == "win32":
    #os.system("gcc editor.c app.o -L./ -llibtcc -o dist/editor.exe")
    os.system("gcc include/glad/glad.c -o glad.o -Iinclude")