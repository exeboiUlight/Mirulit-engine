#ifndef RGB_H
#define RGB_H

#include "../utils/math/geometry.h"

typedef struct {
    float R, G, B;
} RGB;

#define rgb(r, g, b) ((RGB){ \
    .R = ((float)((r)<0?0:(r)>255?255:(r)) / 255.0f), \
    .G = ((float)((g)<0?0:(g)>255?255:(g)) / 255.0f), \
    .B = ((float)((b)<0?0:(b)>255?255:(b)) / 255.0f) \
})

#define rgb_to_vec3(r, g, b) ((Vec3){ \
    .x = ((float)((r)<0?0:(r)>255?255:(r)) / 255.0f), \
    .y = ((float)((g)<0?0:(g)>255?255:(g)) / 255.0f), \
    .z = ((float)((b)<0?0:(b)>255?255:(b)) / 255.0f) \
})

#endif