#pragma once

#include "components/mesh.h"
#include "components/shader.h"
#include "components/transform.h"

#include "math/Vectors.h"

namespace MirulitEngine {
    class Entity3D {
        public:

            bool visible;

            Entity3D(MirulitComponencts::Transform3D _transform) {}
    };
}