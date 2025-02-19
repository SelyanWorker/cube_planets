#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace cp {

    class Camera {
    public:
        Camera(float fov, float aspectRatio, float near, float far)
        : fov(fov)
        , aspectRatio(aspectRatio)
        , near(near)
        , far(far) {
            UpdateProjectionMatrix();
            UpdateViewMatrix();
        }

        void GetUpVector(const glm::vec3& _upVector);
        void SetPosition(const glm::vec3& _position);
        void SetTarget(const glm::vec3& _target);
        void SetAspectRation(float _aspectRatio);

        const glm::vec3& GetUpVector() const { return upVector; }
        const glm::vec3& GetPosition() const { return position; }
        const glm::vec3& GetTarget() const { return target; }
        const glm::mat4& GetProjectionMatrix() const { return projectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return viewMatrix; }

    private:
        void UpdateProjectionMatrix() {
            projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, near, far);
        }

        void UpdateViewMatrix() {
            viewMatrix = glm::lookAt(position, target, upVector);
        }

    private:
        float fov; // degrees
        float aspectRatio;
        float near;
        float far;

        glm::mat4 projectionMatrix{1};
        glm::mat4 viewMatrix{1};

        glm::vec3 upVector{0, 1, 0};
        glm::vec3 position{0};
        glm::vec3 target{0, 0, 0};
    };

}
