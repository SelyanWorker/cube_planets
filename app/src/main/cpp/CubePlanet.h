#pragma once

#include "DrawableTextured.h"
#include "Texture2D.h"
#include "Geometry.h"

namespace cp {

    class CubePlanet : public DrawableTextured {
    public:
        explicit CubePlanet(const Texture2D::Ptr& texture)
        : DrawableTextured(CreateCube(), texture) {}

        void Update(int64_t elapsedTime) override;

        glm::mat4 CreateModelMatrix() const override;

    private:
        void UpdateOrbitRotation(float rotationFactor);

    private:
        glm::vec2 prevPointerPosition{0};
        float orbitRotation{0};
        bool isPointerMovedOnPrevUpdate{false};
    };

} // cp


