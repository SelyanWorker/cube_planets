#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace cp {

    class Camera {
    public:
        glm::vec3 &GetUpVector() { return upVector; }
        glm::vec3 &GetPosition() { return position; }
        glm::vec3 &GetTarget() { return target; }

        const glm::vec3 &GetUpVector() const { return upVector; }
        const glm::vec3 &GetPosition() const { return position; }
        const glm::vec3 &GetTarget() const { return target; }

        [[nodiscard]] glm::mat4 CreteViewMatrix() const;

    private:
        glm::vec3 upVector{0, 1, 0};
        glm::vec3 position{0};
        glm::vec3 target{0, 0, 0};
    };

}
