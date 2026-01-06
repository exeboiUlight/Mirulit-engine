import os
import sys

command_output_editor = ""
command_output_game = ""

editor = "dist/editor"
files_EDITOR = [
    "editor.c",
    "include/glad/glad.c",
]

# Разные библиотеки для Windows и Linux
if sys.platform == "win32":
    liberys = ["opengl32", "glfw3", "openal32", "gdi32", "libtcc", "imm32", "ole32", "comctl32", "uuid"]
    editor += ".exe"
else:
    liberys = ["GL", "glfw", "freetype", "openal", "lua5.4"]

console = True
debug = True

for i in files_EDITOR:
    command_output_editor += " " + i

for j in liberys:
    command_output_editor += " -l" + j

if sys.platform == "win32":
    # Windows компиляция
    os.system("cls")
    debug_flags = "-g -DDEBUG" if debug else "-O3"
    console_flags = " -Wl,-subsystem,windows " if not console else " "
    
    # Компиляция иконки
    os.system("windres app.rc -o app.o")
    
    # Компиляция редактора
    os.system(f"gcc{console_flags}{debug_flags} {command_output_editor} app.o -o {editor} -Iinclude -Iexternal/imgui -L./ -L./dist -static-libgcc -static-libstdc++")
else:
    # Linux компиляция
    os.system("clear")
    debug_flags = "-g -DDEBUG" if debug else "-O3"
    
    # Дополнительные флаги для Linux
    linux_flags = " -ldl -lpthread"
    
    # Компиляция редактора для Linux
    editor_cmd = f"g++ {debug_flags} {command_output_editor} -o {editor} -Iinclude -Iexternal/imgui -Iexternal/imgui/backends -L./ {linux_flags}"
    print(f"Компиляция редактора: {editor_cmd}")
    os.system(editor_cmd)
    
    # Компиляция игры для Linux
    game_cmd = f"g++ {debug_flags} {command_output_game} -o {game} -Iinclude -L./ {linux_flags}"
    print(f"Компиляция игры: {game_cmd}")
    os.system(game_cmd)