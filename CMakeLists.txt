cmake_minimum_required(VERSION 3.10)
project(MirulitEngine)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Ищем TCC через pkg-config
pkg_search_module(TCC tcc)

if(NOT TCC_FOUND)
    # Если не нашли через pkg-config, ищем вручную
    message(STATUS "TCC not found via pkg-config, searching manually...")
    
    # Ищем библиотеку libtcc
    find_library(TCC_LIBRARY NAMES tcc libtcc PATH_SUFFIXES lib lib64)
    find_path(TCC_INCLUDE_DIR libtcc.h PATH_SUFFIXES tcc)
    
    if(TCC_LIBRARY AND TCC_INCLUDE_DIR)
        message(STATUS "Found TCC library: ${TCC_LIBRARY}")
        message(STATUS "Found TCC include dir: ${TCC_INCLUDE_DIR}")
        set(TCC_FOUND TRUE)
        set(TCC_LIBRARIES ${TCC_LIBRARY})
        set(TCC_INCLUDE_DIRS ${TCC_INCLUDE_DIR})
    else()
        # Проверяем стандартные пути
        if(EXISTS "/usr/include/libtcc.h")
            message(STATUS "Found libtcc.h in /usr/include")
            set(TCC_FOUND TRUE)
            set(TCC_LIBRARIES tcc)  # Просто имя библиотеки для линковки
            set(TCC_INCLUDE_DIRS "/usr/include")
        elseif(EXISTS "/usr/local/include/libtcc.h")
            message(STATUS "Found libtcc.h in /usr/local/include")
            set(TCC_FOUND TRUE)
            set(TCC_LIBRARIES tcc)
            set(TCC_INCLUDE_DIRS "/usr/local/include")
        else()
            message(FATAL_ERROR "TCC library not found. Install with: sudo pacman -S tcc")
        endif()
    endif()
endif()

if(WIN32)
    # Windows specific libraries
    set(PLATFORM_LIBS opengl32 gdi32 winmm ws2_32)
    set(DLL_DIR "dll/windows")
else()
    # Linux/Unix specific libraries
    set(PLATFORM_LIBS GL m dl pthread X11)
    set(DLL_DIR "dll/linux")
endif()

# Create dll directories if they don't exist
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/${DLL_DIR})

# Build GLAD static library
add_library(glad STATIC include/glad/glad.c)
target_include_directories(glad PUBLIC include)

# Build editor (using TCC library)
add_executable(editor src/editor.c)
target_include_directories(editor PRIVATE src ${TCC_INCLUDE_DIRS})
target_link_libraries(editor ${TCC_LIBRARIES} ${PLATFORM_LIBS})  # Добавили линковку с libtcc
target_compile_definitions(editor PRIVATE
    $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>
)