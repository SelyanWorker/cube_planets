#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace cp {

    class Entity {
    public:
        glm::vec3& GetPosition() { return position; }
        glm::vec3& GetRotation() { return rotation; }
        float& GetScale() { return scale; }

        const glm::vec3& GetPosition() const { return position; }
        const glm::vec3& GetRotation() const { return rotation; }
        float GetScale() const { return scale; }

        virtual void Update(int64_t elapsedTime) {}

        virtual glm::mat4 CreateModelMatrix() const;

    protected:
        glm::vec3 position{0};
        glm::vec3 rotation{0};
        float scale{1};
    };

} // cp
