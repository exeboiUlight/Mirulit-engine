#include "tcc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Функция для чтения содержимого файла в строку
char* read_file_to_string(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s' (%s)\n", filename, strerror(errno));
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size <= 0) {
        fprintf(stderr, "Error: File '%s' is empty or invalid size: %ld\n", filename, file_size);
        fclose(file);
        return NULL;
    }
    
    char* content = (char*)malloc(file_size + 1);
    if (!content) {
        fprintf(stderr, "Error: Memory allocation failed for file '%s'\n", filename);
        fclose(file);
        return NULL;
    }
    
    size_t bytes_read = fread(content, 1, file_size, file);
    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "Error: Failed to read file '%s' (read %zu of %ld bytes)\n", 
                filename, bytes_read, file_size);
        free(content);
        fclose(file);
        return NULL;
    }
    
    content[file_size] = '\0';
    fclose(file);
    
    return content;
}

// Функция для проверки существования файла
int file_exists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

// Функция для компиляции одного файла в EXE с помощью TCC, включая app.o
int compile_single_file_to_exe_with_tcc(const char* source_code, const char* source_filename, 
                                        const char* output_filename, const char* resource_obj) {
    TCCState* tcc = tcc_new();
    if (!tcc) {
        fprintf(stderr, "Error: Failed to create TCC compiler instance\n");
        return 1;
    }
    
    tcc_set_options(tcc, "-Wall");
    
    tcc_add_include_path(tcc, "engine/include");
    tcc_add_include_path(tcc, "src");
    tcc_add_include_path(tcc, ".");
    
    tcc_add_library_path(tcc, "engine/lib");
    
    if (tcc_set_output_type(tcc, TCC_OUTPUT_EXE) != 0) {
        fprintf(stderr, "Error: Failed to set output type to EXE\n");
        tcc_delete(tcc);
        return 1;
    }
    
    char* code_with_line_directive = (char*)malloc(strlen(source_code) + 50);
    if (!code_with_line_directive) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        tcc_delete(tcc);
        return 1;
    }
    
    sprintf(code_with_line_directive, "#line 1 \"%s\"\n%s", source_filename, source_code);
    
    if (tcc_compile_string(tcc, code_with_line_directive) != 0) {
        free(code_with_line_directive);
        tcc_delete(tcc);
        return 1;
    }
    
    free(code_with_line_directive);
    
    // Добавляем файл ресурсов app.o если он существует
    if (resource_obj && file_exists(resource_obj)) {
        printf("Adding resource file: %s\n", resource_obj);
        if (tcc_add_file(tcc, resource_obj) != 0) {
            fprintf(stderr, "Warning: Failed to add resource file '%s'\n", resource_obj);
        }
    } else if (resource_obj) {
        fprintf(stderr, "Warning: Resource file '%s' not found, skipping\n", resource_obj);
    }
    
    #ifdef _WIN32
        tcc_add_library(tcc, "kernel32");
        tcc_add_library(tcc, "user32");
        tcc_add_library(tcc, "gdi32");
    #endif
    
    if (tcc_output_file(tcc, output_filename) != 0) {
        tcc_delete(tcc);
        return 1;
    }
    
    tcc_delete(tcc);
    return 0;
}

// Функция для создания директории
int create_directory(const char* path) {
    #ifdef _WIN32
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "mkdir \"%s\" 2>nul", path);
    #else
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "mkdir -p \"%s\" 2>/dev/null", path);
    #endif
    
    return system(cmd);
}

// Основная функция для компиляции только config.c в EXE
int main(int argc, char** argv) {
    const char* source_file = (argc > 1) ? argv[1] : "config.c";
    const char* output_file = (argc > 2) ? argv[2] : "bin/config.exe";
    const char* resource_file = (argc > 3) ? argv[3] : "app.o";
    
    // Если ресурсный файл не указан, используем стандартный app.o
    if (argc <= 3) {
        printf("Using default resource file: app.o\n");
    }
    
    FILE* test_file = fopen(source_file, "r");
    if (!test_file) {
        fprintf(stderr, "Error: Source file '%s' not found\n", source_file);
        return 1;
    }
    fclose(test_file);
    
    char output_dir[256] = {0};
    const char* last_slash = strrchr(output_file, '/');
    #ifdef _WIN32
        const char* last_backslash = strrchr(output_file, '\\');
        if (last_backslash && (!last_slash || last_backslash > last_slash)) {
            last_slash = last_backslash;
        }
    #endif
    
    if (last_slash) {
        size_t dir_len = last_slash - output_file;
        if (dir_len < sizeof(output_dir)) {
            strncpy(output_dir, output_file, dir_len);
            output_dir[dir_len] = '\0';
            create_directory(output_dir);
        }
    }
    
    char* source_code = read_file_to_string(source_file);
    if (!source_code) {
        return 1;
    }
    
    printf("Compiling %s to %s with resource file %s\n", 
           source_file, output_file, resource_file);
    
    int result = compile_single_file_to_exe_with_tcc(source_code, source_file, 
                                                    output_file, resource_file);
    
    free(source_code);
    
    if (result == 0) {
        printf("Compilation successful: %s\n", output_file);
    } else {
        fprintf(stderr, "Compilation failed\n");
    }
    
    return result;
}