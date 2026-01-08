#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

#include "core/graphics/window.h"
#include "core/graphics/rgb.h"
#include "core/graphics/matrix.h"
#include "core/graphics/shader.h"
#include "core/graphics/texture.h"

#include "core/graphics/objects/camera.h"
#include "core/graphics/objects/mesh.h"
#include "core/graphics/objects/object.h"

#include "core/utils/json.h"
#include "core/utils/input/keys.h"
#include "core/utils/input/mouse.h"
#include "core/utils/math/geometry.h"
#include "core/utils/math/time.h"
