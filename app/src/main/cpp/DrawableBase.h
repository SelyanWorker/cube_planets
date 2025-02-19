#pragma once

#include <utility>

#include "Entity.h"
#include "Geometry.h"
#include "Shader.h"

namespace cp {

    class DrawableBase : public Entity {
    public:
        explicit DrawableBase(Geometry::Ptr  geometry)
        : geometry(std::move(geometry)) {}

        virtual void Draw(const Shader& shader) const;

    protected:
        void DrawElements() const;
        void BindGeometry(const Shader& shader) const;
        void UnbindGeometry(const Shader& shader) const;

    private:
        Geometry::Ptr geometry;
    };

} // cp

