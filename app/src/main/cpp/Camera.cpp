#include "Camera.h"

glm::mat4 cp::Camera::CreteViewMatrix() const {
    return glm::lookAt(position, target, upVector);
}