#include <glm/gtx/norm.hpp>

#include "LogBuffer.h"
#include "CubePlanet.h"
#include "Input.h"

namespace cp {

    CubePlanet::CubePlanet(const Texture2D::Ptr &texture,
                           float orbitRadius,
                           float orbitRotationSpeed,
                           float spinSpeed,
                           bool isClockWiseOrbitRotation)
    : DrawableTextured(CreateCube(), texture)
    , orbitRadius(orbitRadius)
    , orbitRotationSpeed(orbitRotationSpeed)
    , spinSpeed(spinSpeed)
    , isClockWiseOrbitRotation(isClockWiseOrbitRotation) {}


    void CubePlanet::Update(int64_t elapsedTime) {
        UpdateOrbitRotation(static_cast<float>(elapsedTime) / 1000.f);
        rotation.y += isClockWiseOrbitRotation ? spinSpeed : -spinSpeed;
    }


    glm::mat4 CubePlanet::CreateModelMatrix() const {
        glm::mat4 orbitRotationMatrix = glm::rotate(glm::mat4(1), glm::radians(orbitRotation), glm::vec3{0.0f, 1.0f, 0.0f});
        orbitRotationMatrix = glm::translate(orbitRotationMatrix, glm::vec3{orbitRadius, 0, 0});
        return orbitRotationMatrix * Entity::CreateModelMatrix();
    }


    void CubePlanet::UpdateOrbitRotation(float rotationFactor) {
        const glm::vec2 curPointerPosition = Input::GetPointerPosition();
        const bool isPointerMoving = curPointerPosition != prevPointerPosition;
        if (!isPointerMoving) {
            isPointerMovedOnPrevUpdate = false;
            return;
        }

        if (isPointerMovedOnPrevUpdate) {
            const glm::vec2 direction =  curPointerPosition - prevPointerPosition;
            float rotation = orbitRotationSpeed * rotationFactor * glm::length(direction);
            if (!isClockWiseOrbitRotation) {
                rotation *= -1;
            }

            orbitRotation += direction.x < 0 ? rotation : -rotation;
            orbitRotation = orbitRotation - static_cast<float>(static_cast<int32_t>(orbitRotation / 360.f) * 360);
        }

        prevPointerPosition = curPointerPosition;
        isPointerMovedOnPrevUpdate = true;
    }

} // cp