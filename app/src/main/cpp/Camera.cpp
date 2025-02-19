#include "Camera.h"

namespace cp {

    void Camera::GetUpVector(const glm::vec3& _upVector) {
        if (upVector != _upVector) {
            upVector = _upVector;
            UpdateViewMatrix();
        }
    }


    void Camera::SetPosition(const glm::vec3& _position) {
        if (position != _position) {
            position = _position;
            UpdateViewMatrix();
        }
    }


    void Camera::SetTarget(const glm::vec3& _target) {
        if (target != _target) {
            target = _target;
            UpdateViewMatrix();
        }
    }


    void Camera::SetAspectRation(float _aspectRatio) {
        if (aspectRatio != _aspectRatio) {
            aspectRatio = _aspectRatio;
            UpdateProjectionMatrix();
        }
    }

}
