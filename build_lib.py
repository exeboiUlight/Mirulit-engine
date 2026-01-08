import os
import sys

# Минимальный рабочий скрипт для MinGW
def create_lib():
    print("Создание библиотеки...")
    
    # 1. Компилируем в объектные файлы
    os.system("gcc -c Mirulit.c -Iinclude -o Mirulit.o")
    os.system("gcc -c include/glad/glad.c -Iinclude -o glad.o")
    
    # 2. Создаем библиотеку с помощью ar (MinGW)
    # Если обычный 'ar' не работает, попробуйте 'mingw32-ar' или 'mingw64-ar'
    os.system("ar rcs dist/lib/libmirulit.a Mirulit.o glad.o")
    
    # 3. Очистка
    os.remove("Mirulit.o")
    os.remove("glad.o")
    
    print("✅ Библиотека создана: dist/lib/libmirulit.a")

if __name__ == "__main__":
    os.makedirs("dist/lib", exist_ok=True)
    create_lib()