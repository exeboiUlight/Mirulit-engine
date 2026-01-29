import os
import sys

if sys.platform == "win32":
    os.system("g++ editor.c app.o -L./ -llibtcc -o dist/compiler.exe")
    os.system("g++ editor_gui.c app.o -L./ -llibtcc -o dist/editor.exe")