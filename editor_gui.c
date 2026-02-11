#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define NK_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_BUTTON_TRIGGER_ON_RELEASE
#define NK_GLFW_GL3_IMPLEMENTATION

#include <nuklear.h>
#include "nuklear_glfw_gl3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/* File system related */
#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #include <io.h>
    #include <shlobj.h>
    #include <process.h>
    #define PATH_SEPARATOR "\\"
    #define PATH_SEPARATOR_CHAR '\\'
    #define popen _popen
    #define pclose _pclose
    #define access _access
    #define mkdir _mkdir
    #define chdir _chdir
    #define getcwd _getcwd
#else
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <dirent.h>
    #include <errno.h>
    #include <libgen.h>
    #include <sys/wait.h>
    #define PATH_SEPARATOR "/"
    #define PATH_SEPARATOR_CHAR '/'
#endif

/* File types for syntax highlighting */
typedef enum {
    FT_UNKNOWN,
    FT_C_SOURCE,
    FT_H_SOURCE,
    FT_CPP_SOURCE,
    FT_HPP_SOURCE,
    FT_MAKEFILE,
    FT_TEXT,
    FT_JSON,
    FT_XML,
    FT_MARKDOWN
} FileType;

/* Build output window */
typedef struct {
    char output[65536];
    int output_len;
    int show_window;
    int build_success;
    int is_building;
    char build_command[1024];
    char build_time[64];
} BuildOutput;

typedef struct Project {
    char name[256];
    char path[1024];
    char version[64];
    int has_git;
    time_t created_at;
    struct Project* next;
} Project;

/* File entry for file manager */
typedef struct FileEntry {
    char name[256];
    char path[1024];
    int is_directory;
    size_t size;
    time_t modified_time;
    struct FileEntry* next;
} FileEntry;

/* Tab in code editor */
typedef struct EditorTab {
    char filename[256];
    char filepath[1024];
    char* content;
    size_t content_size;
    size_t content_capacity;
    int modified;
    int scroll_pos;
    int cursor_pos;
    int selection_start;
    int selection_end;
    struct EditorTab* next;
} EditorTab;

typedef struct {
    Project* projects;
    Project* current_project;
    char new_project_name[256];
    char new_project_location[1024];
    char search_query[256];
    int show_create_project;
    int init_git_repo;
    char status_message[256];
    int show_status;
    time_t status_time;
    float editor_scale;
    int show_about;
    
    /* New fields for editor mode */
    int editor_mode;  /* 0 = project list, 1 = code editor */
    FileEntry* file_list;
    EditorTab* editor_tabs;
    EditorTab* current_tab;
    char current_dir[1024];
    char file_filter[256];
    
    /* Editor settings */
    int show_line_numbers;
    int word_wrap;
    int auto_indent;
    char font_size_str[16];
    int font_size;
    
    /* Editor state */
    char editor_text[65536];  /* Buffer for editing */
    int editor_len;
    
    /* Fonts */
    struct nk_font* jetbrains_font;
    struct nk_font* default_font;
    
    /* Build output */
    BuildOutput build_output;
} ProjectManager;

static ProjectManager g_pm;

/* Utility functions */
static int copy_directory(const char* src, const char* dst) {
#ifdef _WIN32
    WIN32_FIND_DATA find_data;
    HANDLE hFind;
    char src_pattern[MAX_PATH];
    char src_file[MAX_PATH];
    char dst_file[MAX_PATH];
    
    if (_mkdir(dst) != 0 && errno != EEXIST) {
        return 0;
    }
    
    snprintf(src_pattern, sizeof(src_pattern), "%s\\*", src);
    
    hFind = FindFirstFile(src_pattern, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        return 0;
    }
    
    do {
        if (strcmp(find_data.cFileName, ".") == 0 || 
            strcmp(find_data.cFileName, "..") == 0) {
            continue;
        }
        
        snprintf(src_file, sizeof(src_file), "%s\\%s", src, find_data.cFileName);
        snprintf(dst_file, sizeof(dst_file), "%s\\%s", dst, find_data.cFileName);
        
        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!copy_directory(src_file, dst_file)) {
                FindClose(hFind);
                return 0;
            }
        } else {
            FILE* src_fp = fopen(src_file, "rb");
            FILE* dst_fp = fopen(dst_file, "wb");
            
            if (src_fp && dst_fp) {
                char buffer[4096];
                size_t bytes;
                while ((bytes = fread(buffer, 1, sizeof(buffer), src_fp)) > 0) {
                    fwrite(buffer, 1, bytes, dst_fp);
                }
                
                fclose(src_fp);
                fclose(dst_fp);
            } else {
                if (src_fp) fclose(src_fp);
                if (dst_fp) fclose(dst_fp);
                FindClose(hFind);
                return 0;
            }
        }
    } while (FindNextFile(hFind, &find_data) != 0);
    
    FindClose(hFind);
    return 1;
#else
    DIR* dir = opendir(src);
    struct dirent* entry;
    struct stat stat_buf;
    char src_path[1024];
    char dst_path[1024];
    
    if (!dir) {
        return 0;
    }
    
    if (mkdir(dst, 0755) != 0 && errno != EEXIST) {
        closedir(dir);
        return 0;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || 
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
        snprintf(dst_path, sizeof(dst_path), "%s/%s", dst, entry->d_name);
        
        if (stat(src_path, &stat_buf) == 0) {
            if (S_ISDIR(stat_buf.st_mode)) {
                if (!copy_directory(src_path, dst_path)) {
                    closedir(dir);
                    return 0;
                }
            } else {
                FILE* src_fp = fopen(src_path, "rb");
                FILE* dst_fp = fopen(dst_path, "wb");
                
                if (src_fp && dst_fp) {
                    char buffer[4096];
                    size_t bytes;
                    while ((bytes = fread(buffer, 1, sizeof(buffer), src_fp)) > 0) {
                        fwrite(buffer, 1, bytes, dst_fp);
                    }
                    
                    fclose(src_fp);
                    fclose(dst_fp);
                } else {
                    if (src_fp) fclose(src_fp);
                    if (dst_fp) fclose(dst_fp);
                    closedir(dir);
                    return 0;
                }
            }
        }
    }
    
    closedir(dir);
    return 1;
#endif
}

static FileType get_file_type(const char* filename) {
    const char* ext = strrchr(filename, '.');
    if (!ext) {
        if (strstr(filename, "Makefile") || strstr(filename, "makefile")) {
            return FT_MAKEFILE;
        }
        return FT_TEXT;
    }
    
    ext++;
    
    if (strcmp(ext, "c") == 0) return FT_C_SOURCE;
    if (strcmp(ext, "h") == 0) return FT_H_SOURCE;
    if (strcmp(ext, "cpp") == 0 || strcmp(ext, "cc") == 0 || strcmp(ext, "cxx") == 0) return FT_CPP_SOURCE;
    if (strcmp(ext, "hpp") == 0 || strcmp(ext, "hh") == 0 || strcmp(ext, "hxx") == 0) return FT_HPP_SOURCE;
    if (strcmp(ext, "txt") == 0) return FT_TEXT;
    if (strcmp(ext, "json") == 0) return FT_JSON;
    if (strcmp(ext, "xml") == 0) return FT_XML;
    if (strcmp(ext, "md") == 0) return FT_MARKDOWN;
    
    return FT_UNKNOWN;
}

static void clean_text_buffer(char* text, int len) {
    /* Remove any non-printable characters except newline, tab, carriage return */
    for (int i = 0; i < len; i++) {
        if (text[i] < 32 && text[i] != '\n' && text[i] != '\r' && text[i] != '\t') {
            text[i] = ' ';
        }
        /* Fix common artifacts */
        if (text[i] == '?') {
            /* Check if it's an artifact (surrounded by whitespace or at line end) */
            int prev_char = (i > 0) ? text[i-1] : 0;
            int next_char = (i < len - 1) ? text[i+1] : 0;
            if (isspace(prev_char) || prev_char == 0 || isspace(next_char) || next_char == 0) {
                text[i] = ' '; /* Replace artifact with space */
            }
        }
    }
}

static void init_project_manager(ProjectManager* pm) {
    pm->projects = NULL;
    pm->current_project = NULL;
    memset(pm->new_project_name, 0, sizeof(pm->new_project_name));
    memset(pm->new_project_location, 0, sizeof(pm->new_project_location));
    memset(pm->search_query, 0, sizeof(pm->search_query));
    memset(pm->status_message, 0, sizeof(pm->status_message));
    pm->show_create_project = 0;
    pm->init_git_repo = 0;
    pm->show_status = 0;
    pm->status_time = 0;
    pm->editor_scale = 1.0f;
    pm->show_about = 0;
    
    /* Editor mode initialization */
    pm->editor_mode = 0;
    pm->file_list = NULL;
    pm->editor_tabs = NULL;
    pm->current_tab = NULL;
    strcpy(pm->current_dir, ".");
    strcpy(pm->file_filter, "*");
    
    /* Editor settings */
    pm->show_line_numbers = 1;
    pm->word_wrap = 0;
    pm->auto_indent = 1;
    strcpy(pm->font_size_str, "14");
    pm->font_size = 14;
    
    /* Editor buffer */
    memset(pm->editor_text, 0, sizeof(pm->editor_text));
    pm->editor_len = 0;
    
    /* Fonts */
    pm->jetbrains_font = NULL;
    pm->default_font = NULL;
    
    /* Build output */
    memset(&pm->build_output, 0, sizeof(BuildOutput));
    pm->build_output.show_window = 0;
    pm->build_output.is_building = 0;
    
    /* Set default project location */
    strcpy(pm->new_project_location, "projects");
}

static void show_status_message(ProjectManager* pm, const char* message) {
    strncpy(pm->status_message, message, sizeof(pm->status_message) - 1);
    pm->show_status = 1;
    pm->status_time = time(NULL);
}

/* Build functions */
/* Build functions - UPDATED VERSION */
static void run_build_script(ProjectManager* pm) {
    system("build.bat");
}

static Project* create_project(const char* name, const char* location, int init_git) {
    char project_path[1024];
    char template_path[1024];
    
    snprintf(project_path, sizeof(project_path), "%s%s%s", 
             location, PATH_SEPARATOR, name);
    
    if (access(project_path, 0) == 0) {
        show_status_message(&g_pm, "Error: Project already exists!");
        return NULL;
    }
    
    if (access(location, 0) != 0) {
#ifdef _WIN32
        mkdir(location);
#else
        mkdir(location, 0755);
#endif
    }
    
    strcpy(template_path, "example");
    if (access(template_path, 0) != 0) {
        show_status_message(&g_pm, "Error: 'example' template not found!");
        return NULL;
    }
    
    if (!copy_directory(template_path, project_path)) {
        show_status_message(&g_pm, "Error: Failed to copy from example!");
        return NULL;
    }
    
    /* Create default build.bat if it doesn't exist */
    char build_bat_path[1024];
    snprintf(build_bat_path, sizeof(build_bat_path), "%s%sbuild.bat", 
             project_path, PATH_SEPARATOR);
    
    if (access(build_bat_path, 0) != 0) {
        FILE* bat = fopen(build_bat_path, "w");
        if (bat) {
            fprintf(bat, "@echo off\n");
            fprintf(bat, "echo Building %s...\n", name);
            fprintf(bat, "echo Project: %s\n", name);
            fprintf(bat, "echo Build started at: %%time%%\n");
            fprintf(bat, "echo.\n");
            fprintf(bat, "REM Add your build commands here\n");
            fprintf(bat, "REM Example for GCC:\n");
            fprintf(bat, "REM gcc main.c -o %s.exe\n", name);
            fprintf(bat, "echo.\n");
            fprintf(bat, "echo Build completed!\n");
            fclose(bat);
        }
    }
    
    Project* project = (Project*)malloc(sizeof(Project));
    if (!project) {
        show_status_message(&g_pm, "Error: Memory allocation failed!");
        return NULL;
    }
    
    strncpy(project->name, name, sizeof(project->name) - 1);
    strncpy(project->path, project_path, sizeof(project->path) - 1);
    strncpy(project->version, "v1.0.0", sizeof(project->version) - 1);
    project->has_git = 0;
    project->created_at = time(NULL);
    project->next = NULL;
    
    if (init_git) {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        chdir(project_path);
        
        system("git init");
        system("git add .");
        system("git commit -m \"Initial commit\"");
        
        project->has_git = 1;
        chdir(cwd);
    }
    
    return project;
}

static void add_project_to_list(ProjectManager* pm, Project* project) {
    if (!pm->projects) {
        pm->projects = project;
    } else {
        Project* current = pm->projects;
        while (current->next) {
            current = current->next;
        }
        current->next = project;
    }
}

static void free_projects(Project* projects) {
    Project* current = projects;
    while (current) {
        Project* next = current->next;
        free(current);
        current = next;
    }
}

static void scan_projects_directory(ProjectManager* pm, const char* directory) {
    free_projects(pm->projects);
    pm->projects = NULL;
    pm->current_project = NULL;
    
    if (access(directory, 0) != 0) {
        return;
    }
    
#ifdef _WIN32
    WIN32_FIND_DATA find_data;
    HANDLE hFind;
    char pattern[MAX_PATH];
    
    snprintf(pattern, sizeof(pattern), "%s\\*", directory);
    
    hFind = FindFirstFile(pattern, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }
    
    do {
        if (strcmp(find_data.cFileName, ".") == 0 || 
            strcmp(find_data.cFileName, "..") == 0) {
            continue;
        }
        
        char project_path[MAX_PATH];
        snprintf(project_path, sizeof(project_path), "%s\\%s", directory, find_data.cFileName);
        
        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            Project* project = (Project*)malloc(sizeof(Project));
            if (project) {
                strncpy(project->name, find_data.cFileName, sizeof(project->name) - 1);
                strncpy(project->path, project_path, sizeof(project->path) - 1);
                strncpy(project->version, "v1.0.0", sizeof(project->version) - 1);
                project->has_git = 0;
                project->created_at = time(NULL);
                project->next = NULL;
                
                char git_path[MAX_PATH];
                snprintf(git_path, sizeof(git_path), "%s\\.git", project_path);
                if (access(git_path, 0) == 0) {
                    project->has_git = 1;
                }
                
                add_project_to_list(pm, project);
            }
        }
    } while (FindNextFile(hFind, &find_data) != 0);
    
    FindClose(hFind);
#else
    DIR* dir = opendir(directory);
    struct dirent* entry;
    struct stat stat_buf;
    
    if (!dir) {
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || 
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char project_path[1024];
        snprintf(project_path, sizeof(project_path), "%s/%s", directory, entry->d_name);
        
        if (stat(project_path, &stat_buf) == 0 && S_ISDIR(stat_buf.st_mode)) {
            Project* project = (Project*)malloc(sizeof(Project));
            if (project) {
                strncpy(project->name, entry->d_name, sizeof(project->name) - 1);
                strncpy(project->path, project_path, sizeof(project->path) - 1);
                strncpy(project->version, "v1.0.0", sizeof(project->version) - 1);
                project->has_git = 0;
                project->created_at = time(NULL);
                project->next = NULL;
                
                char git_path[1024];
                snprintf(git_path, sizeof(git_path), "%s/.git", project_path);
                if (access(git_path, 0) == 0) {
                    project->has_git = 1;
                }
                
                add_project_to_list(pm, project);
            }
        }
    }
    
    closedir(dir);
#endif
}

static void free_file_list(FileEntry* list) {
    FileEntry* current = list;
    while (current) {
        FileEntry* next = current->next;
        free(current);
        current = next;
    }
}

static void free_editor_tabs(EditorTab* tabs) {
    EditorTab* current = tabs;
    while (current) {
        EditorTab* next = current->next;
        if (current->content) free(current->content);
        free(current);
        current = next;
    }
}

static void scan_directory(ProjectManager* pm, const char* path) {
    free_file_list(pm->file_list);
    pm->file_list = NULL;
    
    strcpy(pm->current_dir, path);
    
#ifdef _WIN32
    WIN32_FIND_DATA find_data;
    HANDLE hFind;
    char pattern[MAX_PATH];
    
    snprintf(pattern, sizeof(pattern), "%s\\*", path);
    
    hFind = FindFirstFile(pattern, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }
    
    do {
        if (strcmp(find_data.cFileName, ".") == 0 || 
            strcmp(find_data.cFileName, "..") == 0) {
            continue;
        }
        
        if (strcmp(pm->file_filter, "*") != 0) {
            const char* ext = strrchr(find_data.cFileName, '.');
            if (!ext || strcmp(ext + 1, pm->file_filter) != 0) {
                continue;
            }
        }
        
        FileEntry* entry = (FileEntry*)malloc(sizeof(FileEntry));
        if (!entry) continue;
        
        strncpy(entry->name, find_data.cFileName, sizeof(entry->name) - 1);
        
        char file_path[MAX_PATH];
        snprintf(file_path, sizeof(file_path), "%s\\%s", path, find_data.cFileName);
        strncpy(entry->path, file_path, sizeof(entry->path) - 1);
        
        entry->is_directory = (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        entry->size = (find_data.nFileSizeHigh * (MAXDWORD+1)) + find_data.nFileSizeLow;
        
        FILETIME ft = find_data.ftLastWriteTime;
        ULARGE_INTEGER ui;
        ui.LowPart = ft.dwLowDateTime;
        ui.HighPart = ft.dwHighDateTime;
        entry->modified_time = (time_t)((ui.QuadPart - 116444736000000000ULL) / 10000000ULL);
        
        entry->next = pm->file_list;
        pm->file_list = entry;
        
    } while (FindNextFile(hFind, &find_data) != 0);
    
    FindClose(hFind);
#else
    DIR* dir = opendir(path);
    struct dirent* entry;
    struct stat stat_buf;
    
    if (!dir) {
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || 
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        if (strcmp(pm->file_filter, "*") != 0) {
            const char* ext = strrchr(entry->d_name, '.');
            if (!ext || strcmp(ext + 1, pm->file_filter) != 0) {
                continue;
            }
        }
        
        char file_path[1024];
        snprintf(file_path, sizeof(file_path), "%s/%s", path, entry->d_name);
        
        if (stat(file_path, &stat_buf) != 0) {
            continue;
        }
        
        FileEntry* file_entry = (FileEntry*)malloc(sizeof(FileEntry));
        if (!file_entry) continue;
        
        strncpy(file_entry->name, entry->d_name, sizeof(file_entry->name) - 1);
        strncpy(file_entry->path, file_path, sizeof(file_entry->path) - 1);
        
        file_entry->is_directory = S_ISDIR(stat_buf.st_mode);
        file_entry->size = stat_buf.st_size;
        file_entry->modified_time = stat_buf.st_mtime;
        
        file_entry->next = pm->file_list;
        pm->file_list = file_entry;
    }
    
    closedir(dir);
#endif
}

static EditorTab* open_file_in_editor(ProjectManager* pm, const char* filepath) {
    EditorTab* tab = pm->editor_tabs;
    while (tab) {
        if (strcmp(tab->filepath, filepath) == 0) {
            pm->current_tab = tab;
            memset(pm->editor_text, 0, sizeof(pm->editor_text));
            strncpy(pm->editor_text, tab->content, sizeof(pm->editor_text) - 1);
            pm->editor_len = tab->content_size;
            pm->editor_text[pm->editor_len] = '\0';
            
            /* Clean text buffer from artifacts */
            clean_text_buffer(pm->editor_text, pm->editor_len);
            
            return tab;
        }
        tab = tab->next;
    }
    
    FILE* fp = fopen(filepath, "rb");
    if (!fp) {
        show_status_message(pm, "Failed to open file");
        return NULL;
    }
    
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    if (file_size >= sizeof(pm->editor_text) - 1) {
        show_status_message(pm, "File too large for editor");
        fclose(fp);
        return NULL;
    }
    
    tab = (EditorTab*)malloc(sizeof(EditorTab));
    if (!tab) {
        fclose(fp);
        return NULL;
    }
    
    memset(tab, 0, sizeof(EditorTab));
    
    const char* filename = strrchr(filepath, PATH_SEPARATOR_CHAR);
    if (filename) {
        filename++;
    } else {
        filename = filepath;
    }
    
    strncpy(tab->filename, filename, sizeof(tab->filename) - 1);
    strncpy(tab->filepath, filepath, sizeof(tab->filepath) - 1);
    
    tab->content_capacity = file_size + 1024;
    tab->content = (char*)malloc(tab->content_capacity);
    if (!tab->content) {
        free(tab);
        fclose(fp);
        return NULL;
    }
    
    tab->content_size = fread(tab->content, 1, file_size, fp);
    tab->content[tab->content_size] = '\0';
    tab->modified = 0;
    tab->scroll_pos = 0;
    tab->cursor_pos = 0;
    tab->selection_start = 0;
    tab->selection_end = 0;
    
    fclose(fp);
    
    memset(pm->editor_text, 0, sizeof(pm->editor_text));
    strncpy(pm->editor_text, tab->content, sizeof(pm->editor_text) - 1);
    pm->editor_len = tab->content_size;
    pm->editor_text[pm->editor_len] = '\0';
    
    /* Clean text buffer from artifacts */
    clean_text_buffer(pm->editor_text, pm->editor_len);
    
    tab->next = pm->editor_tabs;
    pm->editor_tabs = tab;
    pm->current_tab = tab;
    
    char status_msg[256];
    snprintf(status_msg, sizeof(status_msg), "Opened: %s (%zu bytes)", filename, tab->content_size);
    show_status_message(pm, status_msg);
    
    return tab;
}

static void save_current_tab(ProjectManager* pm) {
    if (!pm->current_tab) return;
    
    /* Clean text before saving */
    clean_text_buffer(pm->editor_text, pm->editor_len);
    
    FILE* fp = fopen(pm->current_tab->filepath, "wb");
    if (!fp) {
        show_status_message(pm, "Failed to save file");
        return;
    }
    
    free(pm->current_tab->content);
    pm->current_tab->content = (char*)malloc(pm->editor_len + 1);
    if (pm->current_tab->content) {
        memcpy(pm->current_tab->content, pm->editor_text, pm->editor_len);
        pm->current_tab->content[pm->editor_len] = '\0';
        pm->current_tab->content_size = pm->editor_len;
        pm->current_tab->content_capacity = pm->editor_len + 1;
    }
    
    fwrite(pm->editor_text, 1, pm->editor_len, fp);
    fclose(fp);
    
    pm->current_tab->modified = 0;
    
    char status_msg[256];
    snprintf(status_msg, sizeof(status_msg), "Saved: %s", pm->current_tab->filename);
    show_status_message(pm, status_msg);
}

static void close_current_tab(ProjectManager* pm) {
    if (!pm->current_tab) return;
    
    EditorTab** prev = &pm->editor_tabs;
    EditorTab* current = pm->editor_tabs;
    
    while (current) {
        if (current == pm->current_tab) {
            *prev = current->next;
            
            if (current->content) free(current->content);
            free(current);
            
            pm->current_tab = pm->editor_tabs;
            if (pm->current_tab) {
                memset(pm->editor_text, 0, sizeof(pm->editor_text));
                strncpy(pm->editor_text, pm->current_tab->content, sizeof(pm->editor_text) - 1);
                pm->editor_len = pm->current_tab->content_size;
                pm->editor_text[pm->editor_len] = '\0';
                clean_text_buffer(pm->editor_text, pm->editor_len);
            } else {
                pm->editor_text[0] = '\0';
                pm->editor_len = 0;
            }
            return;
        }
        prev = &current->next;
        current = current->next;
    }
}

/* UI Theme */
static void apply_dark_theme(struct nk_context* ctx) {
    struct nk_color table[NK_COLOR_COUNT];
    
    table[NK_COLOR_TEXT] = nk_rgb(220, 220, 220);
    table[NK_COLOR_WINDOW] = nk_rgb(30, 30, 35);
    table[NK_COLOR_HEADER] = nk_rgb(40, 40, 45);
    table[NK_COLOR_BORDER] = nk_rgb(50, 50, 55);
    table[NK_COLOR_BUTTON] = nk_rgb(60, 60, 70);
    table[NK_COLOR_BUTTON_HOVER] = nk_rgb(80, 80, 90);
    table[NK_COLOR_BUTTON_ACTIVE] = nk_rgb(100, 100, 120);
    table[NK_COLOR_TOGGLE] = nk_rgb(60, 60, 70);
    table[NK_COLOR_TOGGLE_HOVER] = nk_rgb(80, 80, 90);
    table[NK_COLOR_TOGGLE_CURSOR] = nk_rgb(0, 150, 200);
    table[NK_COLOR_SELECT] = nk_rgb(60, 60, 70);
    table[NK_COLOR_SELECT_ACTIVE] = nk_rgb(0, 150, 200);
    table[NK_COLOR_SLIDER] = nk_rgb(60, 60, 70);
    table[NK_COLOR_SLIDER_CURSOR] = nk_rgb(0, 150, 200);
    table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgb(0, 180, 230);
    table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgb(0, 120, 170);
    table[NK_COLOR_PROPERTY] = nk_rgb(40, 40, 45);
    table[NK_COLOR_EDIT] = nk_rgb(35, 35, 40);
    table[NK_COLOR_EDIT_CURSOR] = nk_rgb(220, 220, 220);
    table[NK_COLOR_COMBO] = nk_rgb(40, 40, 45);
    table[NK_COLOR_CHART] = nk_rgb(60, 60, 70);
    table[NK_COLOR_CHART_COLOR] = nk_rgb(0, 150, 200);
    table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgb(0, 200, 255);
    table[NK_COLOR_SCROLLBAR] = nk_rgb(40, 40, 45);
    table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgb(60, 60, 70);
    table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgb(80, 80, 90);
    table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgb(100, 100, 120);
    table[NK_COLOR_TAB_HEADER] = nk_rgb(0, 150, 200);
    
    nk_style_from_table(ctx, table);
    
    struct nk_style_button* button = &ctx->style.button;
    button->rounding = 5.0f;
    button->border = 1.0f;
    button->padding = nk_vec2(10, 5);
    
    ctx->style.window.spacing = nk_vec2(10, 10);
    ctx->style.window.padding = nk_vec2(15, 15);
}

/* UI Drawing Functions */
static void draw_sidebar(struct nk_context* ctx, int height) {
    float sidebar_width = 250.0f * g_pm.editor_scale;
    
    if (nk_begin(ctx, "Sidebar", nk_rect(0, 0, sidebar_width, height),
        NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {
        
        nk_layout_row_dynamic(ctx, 60 * g_pm.editor_scale, 1);
        nk_label_colored(ctx, "Project Hub", NK_TEXT_CENTERED, nk_rgb(0, 150, 200));
        
        if (g_pm.editor_mode == 0) {
            nk_layout_row_dynamic(ctx, 30 * g_pm.editor_scale, 1);
            nk_label(ctx, "Search projects...", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 30 * g_pm.editor_scale, 1);
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, 
                                          g_pm.search_query, 
                                          sizeof(g_pm.search_query), 
                                          nk_filter_default);
            
            nk_layout_row_dynamic(ctx, 20 * g_pm.editor_scale, 1);
            
            nk_layout_row_dynamic(ctx, 25 * g_pm.editor_scale, 1);
            nk_label_colored(ctx, "PROJECTS", NK_TEXT_LEFT, nk_rgb(0, 150, 200));
            
            nk_layout_row_dynamic(ctx, 35 * g_pm.editor_scale, 1);
            if (nk_button_label(ctx, "New Project")) {
                g_pm.show_create_project = 1;
                memset(g_pm.new_project_name, 0, sizeof(g_pm.new_project_name));
            }
            
            nk_layout_row_dynamic(ctx, 35 * g_pm.editor_scale, 1);
            if (nk_button_label(ctx, "Browse Projects")) {
                show_status_message(&g_pm, "Browse functionality coming soon!");
            }
        } else {
            nk_layout_row_dynamic(ctx, 35 * g_pm.editor_scale, 1);
            if (nk_button_label(ctx, "← Back to Projects")) {
                g_pm.editor_mode = 0;
                free_file_list(g_pm.file_list);
                g_pm.file_list = NULL;
                free_editor_tabs(g_pm.editor_tabs);
                g_pm.editor_tabs = NULL;
                g_pm.current_tab = NULL;
                g_pm.editor_text[0] = '\0';
                g_pm.editor_len = 0;
                g_pm.build_output.show_window = 0;
            }
            
            nk_layout_row_dynamic(ctx, 20 * g_pm.editor_scale, 1);
            
            nk_layout_row_dynamic(ctx, 25 * g_pm.editor_scale, 1);
            nk_label_colored(ctx, "FILES", NK_TEXT_LEFT, nk_rgb(0, 150, 200));
            
            nk_layout_row_dynamic(ctx, 25 * g_pm.editor_scale, 1);
            nk_label(ctx, "Current Directory:", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 30 * g_pm.editor_scale, 1);
            nk_label(ctx, g_pm.current_dir, NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 35 * g_pm.editor_scale, 1);
            if (nk_button_label(ctx, "Refresh")) {
                scan_directory(&g_pm, g_pm.current_dir);
            }
            
            nk_layout_row_dynamic(ctx, 25 * g_pm.editor_scale, 1);
            nk_label(ctx, "Filter by extension:", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 30 * g_pm.editor_scale, 1);
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, 
                                          g_pm.file_filter, 
                                          sizeof(g_pm.file_filter), 
                                          nk_filter_default);
            
            /* Font settings */
            nk_layout_row_dynamic(ctx, 25 * g_pm.editor_scale, 1);
            nk_label_colored(ctx, "EDITOR", NK_TEXT_LEFT, nk_rgb(0, 150, 200));
            
            nk_layout_row_dynamic(ctx, 25 * g_pm.editor_scale, 1);
            nk_label(ctx, "Font size:", NK_TEXT_LEFT);
            
            nk_layout_row_begin(ctx, NK_STATIC, 30 * g_pm.editor_scale, 2);
            nk_layout_row_push(ctx, 80 * g_pm.editor_scale);
            nk_edit_string_zero_terminated(ctx, NK_EDIT_SIMPLE, 
                                          g_pm.font_size_str, 
                                          sizeof(g_pm.font_size_str), 
                                          nk_filter_decimal);
            
            nk_layout_row_push(ctx, 80 * g_pm.editor_scale);
            if (nk_button_label(ctx, "Apply")) {
                int new_size = atoi(g_pm.font_size_str);
                if (new_size >= 8 && new_size <= 24) {
                    g_pm.font_size = new_size;
                    char msg[64];
                    snprintf(msg, sizeof(msg), "Font size: %dpx (requires restart)", new_size);
                    show_status_message(&g_pm, msg);
                }
            }
            nk_layout_row_end(ctx);
        }
        
        nk_layout_row_dynamic(ctx, 25 * g_pm.editor_scale, 1);
        nk_label(ctx, "UI Scale", NK_TEXT_LEFT);
        
        nk_layout_row_dynamic(ctx, 25 * g_pm.editor_scale, 1);
        char scale_label[32];
        snprintf(scale_label, sizeof(scale_label), "%.0f%%", g_pm.editor_scale * 100);
        nk_label(ctx, scale_label, NK_TEXT_RIGHT);
        
        nk_layout_row_dynamic(ctx, 25 * g_pm.editor_scale, 1);
        nk_slider_float(ctx, 0.5f, &g_pm.editor_scale, 2.0f, 0.1f);
        
        nk_layout_row_dynamic(ctx, 35 * g_pm.editor_scale, 1);
        if (nk_button_label(ctx, "About")) {
            g_pm.show_about = 1;
        }
    }
    nk_end(ctx);
}

static void draw_build_output_window(struct nk_context* ctx, int width, int height) {
    if (g_pm.build_output.show_window) {
        float window_width = 800.0f * g_pm.editor_scale;
        float window_height = 500.0f * g_pm.editor_scale;
        
        if (nk_begin(ctx, "Build Output", 
                    nk_rect(width/2 - window_width/2, height/2 - window_height/2, 
                           window_width, window_height),
                    NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCROLL_AUTO_HIDE)) {
            
            /* Title with build status */
            nk_layout_row_dynamic(ctx, 40 * g_pm.editor_scale, 1);
            if (g_pm.build_output.is_building) {
                nk_label_colored(ctx, "⚙ Building...", NK_TEXT_LEFT, nk_rgb(255, 255, 100));
            } else if (g_pm.build_output.build_success) {
                nk_label_colored(ctx, "✓ Build Successful", NK_TEXT_LEFT, nk_rgb(0, 200, 100));
            } else {
                nk_label_colored(ctx, "✗ Build Failed", NK_TEXT_LEFT, nk_rgb(255, 100, 100));
            }
            
            /* Build output text */
            nk_layout_row_dynamic(ctx, window_height - 120 * g_pm.editor_scale, 1);
            
            if (nk_group_begin(ctx, "BuildLog", NK_WINDOW_BORDER | NK_WINDOW_SCROLL_AUTO_HIDE)) {
                nk_layout_row_dynamic(ctx, 20 * g_pm.editor_scale, 1);
                
                /* Color-code build output */
                char* line = g_pm.build_output.output;
                char* end = line + g_pm.build_output.output_len;
                char* next_line;
                
                while (line < end) {
                    next_line = strchr(line, '\n');
                    if (!next_line) next_line = end;
                    
                    int line_len = next_line - line;
                    if (line_len > 0) {
                        char temp[1024];
                        int copy_len = line_len < sizeof(temp) - 1 ? line_len : sizeof(temp) - 1;
                        strncpy(temp, line, copy_len);
                        temp[copy_len] = '\0';
                        
                        /* Color based on content */
                        if (strstr(temp, "ERROR") || strstr(temp, "Error") || strstr(temp, "error")) {
                            nk_label_colored(ctx, temp, NK_TEXT_LEFT, nk_rgb(255, 100, 100));
                        } else if (strstr(temp, "WARNING") || strstr(temp, "Warning") || strstr(temp, "warning")) {
                            nk_label_colored(ctx, temp, NK_TEXT_LEFT, nk_rgb(255, 200, 100));
                        } else if (strstr(temp, "SUCCESS") || strstr(temp, "Success") || strstr(temp, "success")) {
                            nk_label_colored(ctx, temp, NK_TEXT_LEFT, nk_rgb(100, 255, 100));
                        } else {
                            nk_label(ctx, temp, NK_TEXT_LEFT);
                        }
                    }
                    
                    line = next_line + 1;
                }
                
                nk_group_end(ctx);
            }
            
            /* Buttons */
            nk_layout_row_begin(ctx, NK_STATIC, 40 * g_pm.editor_scale, 3);
            nk_layout_row_push(ctx, 100 * g_pm.editor_scale);
            if (nk_button_label(ctx, "Build Again")) {
                run_build_script(&g_pm);
            }
            
            nk_layout_row_push(ctx, 100 * g_pm.editor_scale);
            if (nk_button_label(ctx, "Clear")) {
                memset(g_pm.build_output.output, 0, sizeof(g_pm.build_output.output));
                g_pm.build_output.output_len = 0;
            }
            
            nk_layout_row_push(ctx, 100 * g_pm.editor_scale);
            if (nk_button_label(ctx, "Close")) {
                g_pm.build_output.show_window = 0;
            }
            nk_layout_row_end(ctx);
        }
        nk_end(ctx);
    }
}

static void draw_main_content(struct nk_context* ctx, int width, int height) {
    float sidebar_width = 250.0f * g_pm.editor_scale;
    float content_width = width - sidebar_width;
    
    if (g_pm.editor_mode == 0) {
        if (nk_begin(ctx, "MainContent", nk_rect(sidebar_width, 0, content_width, height),
            NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {
            
            nk_layout_row_dynamic(ctx, 40 * g_pm.editor_scale, 1);
            nk_label_colored(ctx, "Your Projects", NK_TEXT_LEFT, nk_rgb(220, 220, 220));
            
            float project_height = 120.0f * g_pm.editor_scale;
            int projects_per_row = (int)(content_width / (300.0f * g_pm.editor_scale));
            if (projects_per_row < 1) projects_per_row = 1;
            
            Project* current = g_pm.projects;
            int project_count = 0;
            
            while (current) {
                if (strlen(g_pm.search_query) > 0) {
                    if (strstr(current->name, g_pm.search_query) == NULL) {
                        current = current->next;
                        continue;
                    }
                }
                
                if (project_count % projects_per_row == 0) {
                    nk_layout_row_begin(ctx, NK_DYNAMIC, project_height, projects_per_row);
                }
                
                nk_layout_row_push(ctx, 1.0f/projects_per_row);
                
                if (nk_group_begin(ctx, current->name, NK_WINDOW_BORDER)) {
                    nk_layout_row_dynamic(ctx, 30 * g_pm.editor_scale, 1);
                    char title[256];
                    snprintf(title, sizeof(title), "%s %s", current->name, current->version);
                    nk_label(ctx, title, NK_TEXT_LEFT);
                    
                    nk_layout_row_dynamic(ctx, 40 * g_pm.editor_scale, 1);
                    nk_label(ctx, current->path, NK_TEXT_LEFT);
                    
                    if (current->has_git) {
                        nk_layout_row_dynamic(ctx, 20 * g_pm.editor_scale, 1);
                        nk_label_colored(ctx, "✓ Git Repository", NK_TEXT_LEFT, nk_rgb(0, 200, 100));
                    }
                    
                    nk_layout_row_begin(ctx, NK_STATIC, 30 * g_pm.editor_scale, 2);
                    nk_layout_row_push(ctx, 120 * g_pm.editor_scale);
                    if (nk_button_label(ctx, "Open in Editor")) {
                        g_pm.current_project = current;
                        g_pm.editor_mode = 1;
                        scan_directory(&g_pm, current->path);
                        
                        char message[256];
                        snprintf(message, sizeof(message), "Opening project: %s", current->name);
                        show_status_message(&g_pm, message);
                    }
                    
                    nk_layout_row_push(ctx, 80 * g_pm.editor_scale);
                    if (nk_button_label(ctx, "Delete")) {
                        show_status_message(&g_pm, "Delete functionality coming soon!");
                    }
                    nk_layout_row_end(ctx);
                    
                    nk_group_end(ctx);
                }
                
                project_count++;
                current = current->next;
                
                if (project_count % projects_per_row == 0 || current == NULL) {
                    nk_layout_row_end(ctx);
                }
            }
            
            if (project_count == 0) {
                nk_layout_row_dynamic(ctx, 100 * g_pm.editor_scale, 1);
                nk_label_colored(ctx, "No projects found. Create your first project!", 
                               NK_TEXT_CENTERED, nk_rgb(150, 150, 150));
            }
        }
        nk_end(ctx);
    } else {
        float file_list_width = 300.0f * g_pm.editor_scale;
        float editor_width = content_width - file_list_width;
        
        /* File Manager Panel */
        if (nk_begin(ctx, "FileManager", nk_rect(sidebar_width, 0, file_list_width, height),
            NK_WINDOW_BORDER | NK_WINDOW_SCROLL_AUTO_HIDE)) {
            
            nk_layout_row_dynamic(ctx, 40 * g_pm.editor_scale, 1);
            nk_label_colored(ctx, "File Browser", NK_TEXT_CENTERED, nk_rgb(0, 150, 200));
            
            nk_layout_row_dynamic(ctx, 30 * g_pm.editor_scale, 1);
            if (nk_button_label(ctx, "📁 .. (Parent)")) {
                char parent_dir[1024];
                strcpy(parent_dir, g_pm.current_dir);
                
                #ifdef _WIN32
                    char* sep = strrchr(parent_dir, '\\');
                #else
                    char* sep = strrchr(parent_dir, '/');
                #endif
                
                if (sep && sep != parent_dir) {
                    *sep = '\0';
                    scan_directory(&g_pm, parent_dir);
                } else if (sep == parent_dir) {
                    parent_dir[1] = '\0';
                    scan_directory(&g_pm, parent_dir);
                }
            }
            
            FileEntry* current = g_pm.file_list;
            while (current) {
                char label[512];
                if (current->is_directory) {
                    snprintf(label, sizeof(label), "📁 %s", current->name);
                } else {
                    char size_str[32];
                    if (current->size < 1024) {
                        snprintf(size_str, sizeof(size_str), "%zu B", current->size);
                    } else if (current->size < 1024 * 1024) {
                        snprintf(size_str, sizeof(size_str), "%.1f KB", current->size / 1024.0);
                    } else {
                        snprintf(size_str, sizeof(size_str), "%.1f MB", current->size / (1024.0 * 1024.0));
                    }
                    
                    snprintf(label, sizeof(label), "📄 %s (%s)", current->name, size_str);
                }
                
                nk_layout_row_dynamic(ctx, 35 * g_pm.editor_scale, 1);
                
                if (current->is_directory) {
                    if (nk_button_label(ctx, label)) {
                        scan_directory(&g_pm, current->path);
                    }
                } else {
                    if (nk_button_label(ctx, label)) {
                        open_file_in_editor(&g_pm, current->path);
                    }
                }
                
                current = current->next;
            }
            
            if (!g_pm.file_list) {
                nk_layout_row_dynamic(ctx, 100 * g_pm.editor_scale, 1);
                nk_label(ctx, "No files found", NK_TEXT_CENTERED);
            }
        }
        nk_end(ctx);
        
        /* Editor Panel */
        if (nk_begin(ctx, "CodeEditor", nk_rect(sidebar_width + file_list_width, 0, 
                                               editor_width, height),
            NK_WINDOW_BORDER | NK_WINDOW_SCROLL_AUTO_HIDE)) {
            
            /* Set JetBrains Mono font for editor if available */
            if (g_pm.jetbrains_font) {
                nk_style_set_font(ctx, &g_pm.jetbrains_font->handle);
            }
            
            /* Editor toolbar - добавили кнопку Compile */
            nk_layout_row_begin(ctx, NK_STATIC, 40 * g_pm.editor_scale, 6);
            nk_layout_row_push(ctx, 80 * g_pm.editor_scale);
            if (nk_button_label(ctx, "Save")) {
                save_current_tab(&g_pm);
            }
            
            nk_layout_row_push(ctx, 100 * g_pm.editor_scale);
            if (nk_button_label(ctx, "Save All")) {
                show_status_message(&g_pm, "Save all functionality coming soon!");
            }
            
            nk_layout_row_push(ctx, 80 * g_pm.editor_scale);
            if (nk_button_label(ctx, "Close")) {
                close_current_tab(&g_pm);
            }
            
            /* Кнопка Compile */
            nk_layout_row_push(ctx, 100 * g_pm.editor_scale);
            if (nk_button_label(ctx, "⚙ Compile")) {
                run_build_script(&g_pm);
            }
            
            nk_layout_row_push(ctx, 100 * g_pm.editor_scale);
            nk_checkbox_label(ctx, "Line Numbers", &g_pm.show_line_numbers);
            
            nk_layout_row_push(ctx, 100 * g_pm.editor_scale);
            nk_checkbox_label(ctx, "Word Wrap", &g_pm.word_wrap);
            nk_layout_row_end(ctx);
            
            /* Кнопка для открытия окна сборки */
            nk_layout_row_begin(ctx, NK_STATIC, 30 * g_pm.editor_scale, 2);
            nk_layout_row_push(ctx, 150 * g_pm.editor_scale);
            if (nk_button_label(ctx, "📋 Show Build Output")) {
                g_pm.build_output.show_window = 1;
            }
            
            if (g_pm.build_output.is_building) {
                nk_layout_row_push(ctx, 150 * g_pm.editor_scale);
                nk_label_colored(ctx, "⚙ Building...", NK_TEXT_LEFT, nk_rgb(255, 255, 100));
            }
            nk_layout_row_end(ctx);
            
            /* Tab bar */
            if (g_pm.editor_tabs) {
                nk_layout_row_dynamic(ctx, 40 * g_pm.editor_scale, 1);
                if (nk_group_begin(ctx, "Tabs", NK_WINDOW_BORDER)) {
                    nk_layout_row_begin(ctx, NK_STATIC, 30 * g_pm.editor_scale, 10);
                    
                    EditorTab* tab = g_pm.editor_tabs;
                    while (tab) {
                        char tab_label[256];
                        snprintf(tab_label, sizeof(tab_label), "%s%s", 
                                tab->filename, tab->modified ? " *" : "");
                        
                        nk_layout_row_push(ctx, 120 * g_pm.editor_scale);
                        if (nk_button_label(ctx, tab_label)) {
                            g_pm.current_tab = tab;
                            memset(g_pm.editor_text, 0, sizeof(g_pm.editor_text));
                            strncpy(g_pm.editor_text, tab->content, sizeof(g_pm.editor_text) - 1);
                            g_pm.editor_len = tab->content_size;
                            g_pm.editor_text[g_pm.editor_len] = '\0';
                            clean_text_buffer(g_pm.editor_text, g_pm.editor_len);
                        }
                        
                        tab = tab->next;
                    }
                    nk_layout_row_end(ctx);
                    nk_group_end(ctx);
                }
            }
            
            /* Editor content */
            if (g_pm.current_tab) {
                nk_layout_row_dynamic(ctx, height - 160 * g_pm.editor_scale, 1);
                
                if (nk_group_begin(ctx, "EditorContent", NK_WINDOW_BORDER | NK_WINDOW_SCROLL_AUTO_HIDE)) {
                    
                    /* Clean text buffer before displaying */
                    clean_text_buffer(g_pm.editor_text, g_pm.editor_len);
                    
                    /* File info with font info */
                    nk_layout_row_dynamic(ctx, 20 * g_pm.editor_scale, 1);
                    char info[256];
                    snprintf(info, sizeof(info), "File: %s | Font: JetBrains Mono Bold | Size: %dpx | Lines: ", 
                            g_pm.current_tab->filename, g_pm.font_size);
                    
                    int line_count = 1;
                    for (size_t i = 0; i < g_pm.editor_len; i++) {
                        if (g_pm.editor_text[i] == '\n') line_count++;
                    }
                    char line_str[32];
                    snprintf(line_str, sizeof(line_str), "%d", line_count);
                    strcat(info, line_str);
                    
                    nk_label_colored(ctx, info, NK_TEXT_LEFT, nk_rgb(150, 200, 255));
                    
                    /* Text editor - use SIMPLE instead of BOX to avoid artifacts */
                    nk_layout_row_dynamic(ctx, height - 220 * g_pm.editor_scale, 1);
                    
                    nk_flags flags = NK_EDIT_SIMPLE | NK_EDIT_MULTILINE | NK_EDIT_SELECTABLE;
                    if (g_pm.word_wrap) {
                        flags |= NK_EDIT_CLIPBOARD;
                    }
                    
                    /* Save current text for comparison */
                    char temp_buffer[65536];
                    strcpy(temp_buffer, g_pm.editor_text);
                    
                    /* Edit text */
                    int result = nk_edit_string_zero_terminated(ctx, flags, 
                                                              g_pm.editor_text, 
                                                              sizeof(g_pm.editor_text), 
                                                              nk_filter_default);
                    
                    g_pm.editor_len = strlen(g_pm.editor_text);
                    
                    /* Check if content was modified */
                    if (strcmp(temp_buffer, g_pm.editor_text) != 0) {
                        if (!g_pm.current_tab->modified) {
                            g_pm.current_tab->modified = 1;
                            
                            free(g_pm.current_tab->content);
                            g_pm.current_tab->content = (char*)malloc(g_pm.editor_len + 1);
                            if (g_pm.current_tab->content) {
                                memcpy(g_pm.current_tab->content, g_pm.editor_text, g_pm.editor_len);
                                g_pm.current_tab->content[g_pm.editor_len] = '\0';
                                g_pm.current_tab->content_size = g_pm.editor_len;
                            }
                        }
                    }
                    
                    nk_group_end(ctx);
                }
            } else {
                nk_layout_row_dynamic(ctx, 100 * g_pm.editor_scale, 1);
                nk_label(ctx, "No file open. Click on a file in the file browser to open it.", 
                        NK_TEXT_CENTERED);
                
                nk_layout_row_dynamic(ctx, 20 * g_pm.editor_scale, 1);
                nk_label_colored(ctx, "Using JetBrains Mono Bold font for code editing", 
                               NK_TEXT_CENTERED, nk_rgb(150, 200, 255));
            }
            
            /* Restore default font */
            if (g_pm.default_font) {
                nk_style_set_font(ctx, &g_pm.default_font->handle);
            }
        }
        nk_end(ctx);
    }
}

static void draw_create_project_window(struct nk_context* ctx, int width, int height) {
    if (g_pm.show_create_project) {
        float window_width = 500.0f * g_pm.editor_scale;
        float window_height = 350.0f * g_pm.editor_scale;
        
        if (nk_begin(ctx, "Create New Project", 
                    nk_rect(width/2 - window_width/2, height/2 - window_height/2, 
                           window_width, window_height),
                    NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
            
            nk_layout_row_dynamic(ctx, 40 * g_pm.editor_scale, 1);
            nk_label_colored(ctx, "Create New Project", NK_TEXT_CENTERED, nk_rgb(0, 150, 200));
            
            nk_layout_row_dynamic(ctx, 25 * g_pm.editor_scale, 1);
            nk_label(ctx, "Project Name:", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 35 * g_pm.editor_scale, 1);
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, 
                                          g_pm.new_project_name, 
                                          sizeof(g_pm.new_project_name), 
                                          nk_filter_default);
            
            nk_layout_row_dynamic(ctx, 25 * g_pm.editor_scale, 1);
            nk_label(ctx, "Project Location:", NK_TEXT_LEFT);
            
            nk_layout_row_begin(ctx, NK_STATIC, 35 * g_pm.editor_scale, 2);
            nk_layout_row_push(ctx, window_width - 100 * g_pm.editor_scale);
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, 
                                          g_pm.new_project_location, 
                                          sizeof(g_pm.new_project_location), 
                                          nk_filter_default);
            
            nk_layout_row_push(ctx, 80 * g_pm.editor_scale);
            if (nk_button_label(ctx, "Browse")) {
                show_status_message(&g_pm, "Browse functionality coming soon!");
            }
            nk_layout_row_end(ctx);
            
            nk_layout_row_dynamic(ctx, 40 * g_pm.editor_scale, 1);
            nk_label_colored(ctx, "Note: Project will be created by copying './example/' folder", 
                           NK_TEXT_CENTERED, nk_rgb(150, 200, 255));
            
            nk_layout_row_dynamic(ctx, 30 * g_pm.editor_scale, 1);
            nk_checkbox_label(ctx, "Initialize Git Repository", &g_pm.init_git_repo);
            
            nk_layout_row_begin(ctx, NK_STATIC, 40 * g_pm.editor_scale, 2);
            nk_layout_row_push(ctx, 120 * g_pm.editor_scale);
            if (nk_button_label(ctx, "Create Project")) {
                if (strlen(g_pm.new_project_name) > 0) {
                    Project* new_project = create_project(g_pm.new_project_name, 
                                                         g_pm.new_project_location,
                                                         g_pm.init_git_repo);
                    if (new_project) {
                        add_project_to_list(&g_pm, new_project);
                        g_pm.current_project = new_project;
                        
                        char message[256];
                        snprintf(message, sizeof(message), 
                                "Project '%s' created successfully!", 
                                g_pm.new_project_name);
                        show_status_message(&g_pm, message);
                        
                        g_pm.show_create_project = 0;
                        g_pm.init_git_repo = 0;
                    }
                } else {
                    show_status_message(&g_pm, "Error: Project name cannot be empty!");
                }
            }
            
            nk_layout_row_push(ctx, 100 * g_pm.editor_scale);
            if (nk_button_label(ctx, "Cancel")) {
                g_pm.show_create_project = 0;
                g_pm.init_git_repo = 0;
            }
            nk_layout_row_end(ctx);
        }
        nk_end(ctx);
    }
}

static void draw_about_window(struct nk_context* ctx, int width, int height) {
    if (g_pm.show_about) {
        float window_width = 400.0f * g_pm.editor_scale;
        float window_height = 450.0f * g_pm.editor_scale;
        
        if (nk_begin(ctx, "About Project Hub", 
                    nk_rect(width/2 - window_width/2, height/2 - window_height/2, 
                           window_width, window_height),
                    NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
            
            nk_layout_row_dynamic(ctx, 60 * g_pm.editor_scale, 1);
            nk_label_colored(ctx, "Project Hub", NK_TEXT_CENTERED, nk_rgb(0, 150, 200));
            
            nk_layout_row_dynamic(ctx, 25 * g_pm.editor_scale, 1);
            nk_label(ctx, "Version: 1.0.0", NK_TEXT_CENTERED);
            
            nk_layout_row_dynamic(ctx, 60 * g_pm.editor_scale, 1);
            nk_label(ctx, "Create and manage your game projects", NK_TEXT_CENTERED);
            
            nk_layout_row_dynamic(ctx, 25 * g_pm.editor_scale, 1);
            nk_label(ctx, "Features:", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 20 * g_pm.editor_scale, 1);
            nk_label(ctx, "• Create projects from example template", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 20 * g_pm.editor_scale, 1);
            nk_label(ctx, "• Optional Git repository initialization", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 20 * g_pm.editor_scale, 1);
            nk_label(ctx, "• Project management and organization", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 20 * g_pm.editor_scale, 1);
            nk_label(ctx, "• Integrated code editor with JetBrains Mono", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 20 * g_pm.editor_scale, 1);
            nk_label(ctx, "• File browser for project navigation", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 20 * g_pm.editor_scale, 1);
            nk_label(ctx, "• Edit and save files directly in the editor", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 20 * g_pm.editor_scale, 1);
            nk_label(ctx, "• One-click compile with build.bat", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 20 * g_pm.editor_scale, 1);
            nk_label(ctx, "• Build output window with color coding", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 40 * g_pm.editor_scale, 1);
            if (nk_button_label(ctx, "Close")) {
                g_pm.show_about = 0;
            }
        }
        nk_end(ctx);
    }
}

int main(void) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    GLFWwindow* window = glfwCreateWindow(1400, 800, "Project Hub", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwTerminate();
        return -1;
    }
    
    init_project_manager(&g_pm);
    
    struct nk_glfw glfw = {0};
    struct nk_context* ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);
    if (!ctx) {
        fprintf(stderr, "Failed to initialize Nuklear\n");
        glfwTerminate();
        return -1;
    }
    
    /* Load fonts */
    struct nk_font_atlas* atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);
    
    /* Add default font */
    struct nk_font_config config = nk_font_config(0);
    config.range = nk_font_default_glyph_ranges();
    g_pm.default_font = nk_font_atlas_add_default(atlas, 16, &config);
    
    /* Try to load JetBrains Mono Bold font */
    g_pm.jetbrains_font = nk_font_atlas_add_from_file(atlas, "JetBrainsMono-Bold.ttf", g_pm.font_size, &config);
    
    nk_glfw3_font_stash_end(&glfw);
    
    /* Set default font */
    if (g_pm.default_font) {
        nk_style_set_font(ctx, &g_pm.default_font->handle);
    }
    
    if (g_pm.jetbrains_font) {
        printf("✓ JetBrains Mono Bold font loaded successfully\n");
    } else {
        printf("✗ Failed to load JetBrains Mono Bold font. Make sure JetBrainsMono-Bold.ttf is in the current directory.\n");
        printf("  Using default monospace font instead.\n");
        
        /* Fallback to system monospace font */
        #ifdef _WIN32
            g_pm.jetbrains_font = nk_font_atlas_add_from_file(atlas, "C:/Windows/Fonts/consola.ttf", g_pm.font_size, &config);
        #else
            g_pm.jetbrains_font = nk_font_atlas_add_from_file(atlas, "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", g_pm.font_size, &config);
        #endif
    }
    
    apply_dark_theme(ctx);
    
    scan_projects_directory(&g_pm, g_pm.new_project_location);
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        nk_glfw3_new_frame(&glfw);
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.08f, 0.08f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        draw_sidebar(ctx, height);
        draw_main_content(ctx, width, height);
        draw_create_project_window(ctx, width, height);
        draw_build_output_window(ctx, width, height);
        draw_about_window(ctx, width, height);
        
        if (g_pm.show_status) {
            time_t current_time = time(NULL);
            if (current_time - g_pm.status_time > 3) {
                g_pm.show_status = 0;
            } else {
                float msg_width = 400.0f * g_pm.editor_scale;
                float msg_height = 50.0f * g_pm.editor_scale;
                
                if (nk_begin(ctx, "Status", 
                            nk_rect(width/2 - msg_width/2, height - msg_height - 20, 
                                   msg_width, msg_height),
                            NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {
                    nk_layout_row_dynamic(ctx, msg_height, 1);
                    nk_label_colored(ctx, g_pm.status_message, NK_TEXT_CENTERED, nk_rgb(255, 255, 100));
                }
                nk_end(ctx);
            }
        }
        
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, 1024*1024, 256*1024);
        
        glfwSwapBuffers(window);
    }
    
    nk_glfw3_shutdown(&glfw);
    glfwDestroyWindow(window);
    glfwTerminate();
    
    free_projects(g_pm.projects);
    free_file_list(g_pm.file_list);
    free_editor_tabs(g_pm.editor_tabs);
    
    return 0;
}