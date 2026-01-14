#ifndef MIRULIT_H
#define MIRULIT_H

#include <mirulit/utils.h>

typedef struct {
    void (*Scene)();
    void (*Main_Sript)();
} NirulitScene;

typedef struct {
    void (*Start)();
    void (*Update)();
    void (*End)();
} MirulitScript;

typedef struct {
    void (*Start)();
    void (*Update)();
} MirulitShader;

#endif