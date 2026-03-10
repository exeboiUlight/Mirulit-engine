import os

print("1| debug\n2| release\n3| leave\n4| compile resourses")
mode = int(input("::"))

output_command = "g++ -L./lib -I./include "

output_file = "Mirulit.exe"

source_files = [
    "src/main.cpp",
    "include/glad/glad.c",
]

liberys = [
    "opengl32",
    "glfw3",
    "gdi32",
]

if mode == 4:
    os.system("windres app.rc -o app.o")
    quit()

if mode == 1:
    output_file = "bin/debug/"+output_file
elif mode == 2:
    output_command += "-mwindows "
    output_command += "-O3 "
    output_file = "bin/release/"+output_file

elif mode == 3:
    quit()
else:
    print("Invalid option")
    quit()

if mode == 1 or mode == 2:
    for i in source_files:
        output_command += i+" "
    for i in liberys:
        output_command += "-l"+i+" "
    
    output_command += "-o ./"+output_file+" "
    output_command += "app.o "

print(f"{output_command}")
os.system(output_command)