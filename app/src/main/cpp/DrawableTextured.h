#pragma once

#include "DrawableBase.h"
#include "Texture2D.h"

namespace cp {

    class DrawableTextured : public DrawableBase {
    public:
        DrawableTextured(const Geometry::Ptr& geometry, const Texture2D::Ptr& texture)
        : DrawableBase(geometry)
        , texture(texture) {}

        void Draw(const Shader& shader) const override;

    private:
        Texture2D::Ptr texture;
    };

} // cp
