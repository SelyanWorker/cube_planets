#include "AndroidOut.h"
#include "CubePlanet.h"
#include "Input.h"

namespace cp {

    void CubePlanet::Update(int64_t elapsedTime) {
        UpdateOrbitRotation(static_cast<float>(elapsedTime) / 1000.f);
        rotation.y += 5.f;
    }

    glm::mat4 CubePlanet::CreateModelMatrix() const {
        constexpr float ORBIT_RADIUS = 10.f;

        glm::mat4 orbitRotationMatrix = glm::translate(glm::mat4(1), glm::vec3(ORBIT_RADIUS, 0.0f, 0.0f));
        orbitRotationMatrix = glm::rotate(orbitRotationMatrix, glm::radians(orbitRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        orbitRotationMatrix = glm::translate(orbitRotationMatrix, glm::vec3(-ORBIT_RADIUS, 0.0f, 0.0f));

        return orbitRotationMatrix * Entity::CreateModelMatrix();
    }

    void CubePlanet::UpdateOrbitRotation(float rotationFactor) {
        constexpr float ORBIT_SPEED = 1.0f;

        const glm::vec2 curPointerPosition = Input::GetPointerPosition();
        const bool isPointerMoving = curPointerPosition != prevPointerPosition;
        if (!isPointerMoving) {
            isPointerMovedOnPrevUpdate = false;
            return;
        }

        if (isPointerMovedOnPrevUpdate) {
            const glm::vec2 direction = curPointerPosition - prevPointerPosition;
            if (direction.x < 0) {
                orbitRotation += ORBIT_SPEED * rotationFactor * glm::length(direction);
            } else {
                orbitRotation -= ORBIT_SPEED * rotationFactor * glm::length(direction);
            }
        }

        prevPointerPosition = curPointerPosition;
        isPointerMovedOnPrevUpdate = true;
    }

} // cp