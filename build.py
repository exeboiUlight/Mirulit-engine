import os
import sys

if sys.platform == "win32":
    os.system("gcc editor.c app.o -L./ -llibtcc -o dist/editor.exe")