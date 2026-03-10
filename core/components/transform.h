#pragma once

#include "../math/Vectors.h"

namespace MirulitComponencts {
    class Transform3D {
        public:

            MirulitMath::Vector3f position;
            MirulitMath::Vector3f scale;
            MirulitMath::Vector3f rotate;

            Transform3D(MirulitMath::Vector3f _position, MirulitMath::Vector3f _scale, MirulitMath::Vector3f _rotate) {
                position = _position;
                scale = _scale;
                rotate = _rotate;
            }
    };

    class Transform2D {
        public:

            MirulitMath::Vector3f position;
            MirulitMath::Vector3f scale;
            float rotate;

            Transform2D(MirulitMath::Vector3f _position, MirulitMath::Vector3f _scale, float _rotate) {
                position = _position;
                scale = _scale;
                rotate = _rotate;
            }
    };

}