import os
import sys

if sys.platform == "win32":
    os.system("gcc editor.c app.o -L./ -llibtcc -o dist/compiler.exe")
    os.system("gcc editor_gui.c app.o -lgdi -mwindows -Iinclude -o dist/editor.exe")