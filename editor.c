#include "tcc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <direct.h>
    #include <windows.h>
    #define mkdir(path, mode) _mkdir(path)
    #define popen _popen
    #define pclose _pclose
#else
    #include <unistd.h>
    #include <errno.h>
#endif

// Простая функция для проверки существования файла
int file_exists(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

// Создание директории если её нет
void ensure_directory(const char* path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        #ifdef _WIN32
            _mkdir(path);
        #else
            mkdir(path, 0755);
        #endif
    }
}

// Функция для добавления ВСЕХ библиотек из папки lib
void add_all_library_files(TCCState* tcc, const char* lib_dir) {
    printf("Looking for libraries in: %s\n", lib_dir);
    
    if (!file_exists(lib_dir)) {
        printf("  Directory does not exist\n");
        return;
    }
    
    // Создаем команду для поиска файлов библиотек
    char find_cmd[512];
    
    #ifdef _WIN32
        // На Windows
        snprintf(find_cmd, sizeof(find_cmd), "dir /b \"%s\\*.lib\" \"%s\\*.a\" 2>nul", lib_dir, lib_dir);
        FILE* pipe = popen(find_cmd, "r");
    #else
        // На Linux/Mac
        snprintf(find_cmd, sizeof(find_cmd), "ls \"%s\"/*.a \"%s\"/*.lib 2>/dev/null", lib_dir, lib_dir);
        FILE* pipe = popen(find_cmd, "r");
    #endif
    
    if (pipe) {
        char buffer[256];
        int count = 0;
        
        while (fgets(buffer, sizeof(buffer), pipe)) {
            // Убираем символ новой строки
            buffer[strcspn(buffer, "\r\n")] = 0;
            
            if (buffer[0]) {
                char full_path[512];
                #ifdef _WIN32
                    snprintf(full_path, sizeof(full_path), "%s\\%s", lib_dir, buffer);
                #else
                    snprintf(full_path, sizeof(full_path), "%s/%s", lib_dir, buffer);
                #endif
                
                printf("  Adding: %s\n", buffer);
                
                // Добавляем файл библиотеки
                if (tcc_add_file(tcc, full_path) != 0) {
                    printf("  Warning: Failed to add %s\n", buffer);
                }
                count++;
            }
        }
        
        pclose(pipe);
        
        if (count == 0) {
            printf("  No library files found\n");
        }
    } else {
        printf("  Could not scan directory\n");
    }
}

// Добавление системных библиотек
void add_system_libraries(TCCState* tcc) {
    printf("Adding system libraries\n");
    
    #ifdef _WIN32
        tcc_add_library(tcc, "kernel32");
        tcc_add_library(tcc, "user32");
        tcc_add_library(tcc, "gdi32");
        tcc_add_library(tcc, "opengl32");
        tcc_add_library(tcc, "winmm");
        tcc_add_library(tcc, "ws2_32");
    #else
        tcc_add_library(tcc, "m");
        tcc_add_library(tcc, "dl");
        tcc_add_library(tcc, "pthread");
        tcc_add_library(tcc, "GL");
        tcc_add_library(tcc, "X11");
        
        // Проверяем наличие GLFW
        if (file_exists("/usr/lib/libglfw.so") || 
            file_exists("/usr/local/lib/libglfw.so") ||
            file_exists("/usr/lib/x86_64-linux-gnu/libglfw.so") ||
            file_exists("lib/libglfw.a") ||
            file_exists("lib/libglfw.so")) {
            printf("  Adding: glfw\n");
            tcc_add_library(tcc, "glfw");
        }
        
        // Для MacOS
        #ifdef __APPLE__
            tcc_add_library(tcc, "objc");
            tcc_add_library(tcc, "Cocoa");
            tcc_add_library(tcc, "IOKit");
            tcc_add_library(tcc, "CoreFoundation");
            tcc_add_library(tcc, "CoreVideo");
        #endif
    #endif
}

// Показать справку
void show_help(const char* program_name) {
    printf("TCC Game Compiler\n");
    printf("Usage: %s [options] <source.c>\n\n", program_name);
    printf("Options:\n");
    printf("  -o <file>     Output executable name\n");
    printf("  -r <file>     Resource object file\n");
    printf("  -I <path>     Add include path\n");
    printf("  -L <path>     Add library path\n");
    printf("  -l <lib>      Link with library\n");
    printf("  -v            Verbose output\n");
    printf("  -h, --help    Show this help\n");
    printf("\nExamples:\n");
    printf("  %s game.c\n", program_name);
    printf("  %s -o mygame.exe main.c\n", program_name);
    printf("  %s -I./include -L./lib -lglfw config.c\n", program_name);
}

int main(int argc, char** argv) {
    const char* source_file = "config.c";
    const char* output_file = "bin/game.exe";
    const char* resource_file = "app.o";
    
    // Динамические списки для путей и библиотек
    char** include_paths = NULL;
    char** library_paths = NULL;
    char** libraries = NULL;
    int include_count = 0;
    int library_path_count = 0;
    int library_count = 0;
    int verbose = 0;
    
    // Парсинг аргументов командной строки
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
                output_file = argv[++i];
            }
            else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
                resource_file = argv[++i];
            }
            else if (strcmp(argv[i], "-I") == 0 && i + 1 < argc) {
                include_paths = realloc(include_paths, (include_count + 1) * sizeof(char*));
                include_paths[include_count++] = argv[++i];
            }
            else if (strcmp(argv[i], "-L") == 0 && i + 1 < argc) {
                library_paths = realloc(library_paths, (library_path_count + 1) * sizeof(char*));
                library_paths[library_path_count++] = argv[++i];
            }
            else if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
                libraries = realloc(libraries, (library_count + 1) * sizeof(char*));
                libraries[library_count++] = argv[++i];
            }
            else if (strcmp(argv[i], "-v") == 0) {
                verbose = 1;
            }
            else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                show_help(argv[0]);
                return 0;
            }
            else {
                fprintf(stderr, "Unknown option: %s\n", argv[i]);
                fprintf(stderr, "Use %s --help for usage information\n", argv[0]);
                return 1;
            }
        }
        else {
            // Первый не-опциональный аргумент - исходный файл
            source_file = argv[i];
        }
    }
    
    printf("TCC Compiler\n");
    printf("============\n");
    printf("Source: %s\n", source_file);
    printf("Output: %s\n", output_file);
    printf("Resource: %s\n\n", resource_file);
    
    // Проверяем исходный файл
    if (!file_exists(source_file)) {
        fprintf(stderr, "ERROR: Source file '%s' not found!\n", source_file);
        
        // Проверяем альтернативные имена
        const char* alternatives[] = {"main.c", "game.c", "src/main.c", NULL};
        for (int i = 0; alternatives[i] != NULL; i++) {
            if (file_exists(alternatives[i])) {
                printf("Did you mean: %s %s\n", argv[0], alternatives[i]);
            }
        }
        return 1;
    }
    
    // Создаем выходную директорию
    char output_dir[256];
    strncpy(output_dir, output_file, sizeof(output_dir));
    char* last_slash = strrchr(output_dir, '/');
    if (!last_slash) last_slash = strrchr(output_dir, '\\');
    if (last_slash) {
        *last_slash = '\0';
        ensure_directory(output_dir);
    } else {
        // Если нет пути, создаем папку bin
        ensure_directory("bin");
    }
    
    // Создаем компилятор
    TCCState* tcc = tcc_new();
    if (!tcc) {
        fprintf(stderr, "ERROR: Cannot create TCC instance\n");
        return 1;
    }
    
    // Настройки
    tcc_set_output_type(tcc, TCC_OUTPUT_EXE);
    
    // Добавляем стандартные пути include
    tcc_add_include_path(tcc, "include");
    tcc_add_include_path(tcc, ".");
    tcc_add_include_path(tcc, "include");
    tcc_add_include_path(tcc, "src");
    
    // Добавляем пользовательские пути include
    for (int i = 0; i < include_count; i++) {
        if (verbose) printf("Adding include path: %s\n", include_paths[i]);
        tcc_add_include_path(tcc, include_paths[i]);
    }
    
    // Добавляем стандартные пути для библиотек
    tcc_add_library_path(tcc, "lib");
    tcc_add_library_path(tcc, "lib");
    
    // Добавляем пользовательские пути для библиотек
    for (int i = 0; i < library_path_count; i++) {
        if (verbose) printf("Adding library path: %s\n", library_paths[i]);
        tcc_add_library_path(tcc, library_paths[i]);
    }
    
    // 1. Компилируем исходный файл
    printf("1. Compiling: %s\n", source_file);
    if (tcc_add_file(tcc, source_file) != 0) {
        fprintf(stderr, "ERROR: Failed to compile %s\n", source_file);
        
        // Даем подсказки
        printf("\nPossible issues:\n");
        printf("  - Missing include files in include/\n");
        printf("  - Syntax errors in source code\n");
        printf("  - Required headers not found\n");
        
        tcc_delete(tcc);
        free(include_paths);
        free(library_paths);
        free(libraries);
        return 1;
    }
    
    // 2. Добавляем ресурсы
    if (file_exists(resource_file)) {
        printf("2. Adding resource: %s\n", resource_file);
        if (tcc_add_file(tcc, resource_file) != 0) {
            printf("  Warning: Could not add %s (may be incompatible format)\n", resource_file);
        }
    } else {
        printf("2. Resource file not found: %s\n", resource_file);
    }
    
    // 3. Добавляем библиотеки из lib
    printf("3. Adding libraries from lib\n");
    add_all_library_files(tcc, "lib");
    
    // 4. Добавляем пользовательские библиотеки
    for (int i = 0; i < library_count; i++) {
        printf("  Adding library: %s\n", libraries[i]);
        tcc_add_library(tcc, libraries[i]);
    }
    
    // 5. Добавляем системные библиотеки
    add_system_libraries(tcc);
    
    // 6. Создаем выходной файл
    printf("\n4. Creating output: %s\n", output_file);
    if (tcc_output_file(tcc, output_file) != 0) {
        fprintf(stderr, "ERROR: Failed to create output file\n");
        
        printf("\nTroubleshooting:\n");
        printf("  - Check that all required libraries exist\n");
        printf("  - Verify library paths are correct\n");
        printf("  - Ensure output directory is writable\n");
        
        tcc_delete(tcc);
        free(include_paths);
        free(library_paths);
        free(libraries);
        return 1;
    }
    
    // Очистка
    tcc_delete(tcc);
    free(include_paths);
    free(library_paths);
    free(libraries);
    
    // Проверяем, что файл создан
    if (file_exists(output_file)) {
        printf("\n✅ SUCCESS: Compilation completed!\n");
        printf("   Output: %s\n", output_file);
        
        // Получаем размер файла
        FILE* f = fopen(output_file, "rb");
        if (f) {
            fseek(f, 0, SEEK_END);
            long size = ftell(f);
            fclose(f);
            
            if (size < 1024) 
                printf("   Size: %ld bytes\n", size);
            else if (size < 1024*1024) 
                printf("   Size: %.1f KB\n", size/1024.0);
            else 
                printf("   Size: %.1f MB\n", size/(1024.0*1024.0));
        }
        
        #ifdef _WIN32
            printf("   Run: %s\n", output_file);
        #else
            // Проверяем, есть ли права на выполнение
            if (access(output_file, X_OK) == 0) {
                printf("   Run: ./%s\n", output_file);
            } else {
                printf("   Note: File may not be executable, run: chmod +x %s\n", output_file);
            }
        #endif
    } else {
        fprintf(stderr, "\n❌ ERROR: Output file was not created\n");
        return 1;
    }
    
    return 0;
}