#ifndef MIRULIT_H
#define MIRULIT_H

#include <stdbool.h>
#include <windows.h>

typedef void (*MirulitLoop)(void *(update)());

int MirulitInit() {

    typedef bool (*MirulitInit)(int width, int height, const char* title, const char* icon);

    HINSTANCE hDLL;
    MirulitInit mirInit;

    hDLL = LoadLibrary("mirulit.dll");
    if (hDLL == NULL) {
        printf("Error in load dll file %d\n", GetLastError());
        return 1;
    }

    mirInit = (MirulitInit)GetProcAddress(hDLL, "AddNumbers");
}

#endif