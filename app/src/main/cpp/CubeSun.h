#pragma once

#include "DrawableTextured.h"

namespace cp {

    class CubeSun : public DrawableTextured {
    public:
        explicit CubeSun(const Texture2D::Ptr& texture, float spinSpeed)
        : DrawableTextured(CreateCube(), texture)
        , spinSpeed(spinSpeed) {}

        void Update(int64_t elapsedTime) override;

    private:
        const float spinSpeed;
    };

} // cp
