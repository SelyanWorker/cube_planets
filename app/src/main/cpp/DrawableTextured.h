#pragma once

#include <utility>

#include "DrawableBase.h"
#include "Texture2D.h"

namespace cp {

    class DrawableTextured : public DrawableBase {
    public:
        DrawableTextured(Geometry::Ptr geometry, Texture2D::Ptr texture)
        : DrawableBase(std::move(geometry))
        , texture(std::move(texture)) {}

        void Draw(const Shader& shader) const override;

    private:
        Texture2D::Ptr texture;
    };

} // cp
