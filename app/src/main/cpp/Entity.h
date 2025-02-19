#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace cp {

    class Entity {
    public:
        virtual ~Entity() = default;

        virtual void Update(int64_t elapsedTime) {}

        [[nodiscard]] virtual glm::mat4 CreateModelMatrix() const;

        void SetPosition(const glm::vec3& _position) { position = _position; }
        void SetRotation(const glm::vec3& _rotation) { rotation = _rotation; }
        void SetScale(float _scale) { scale = _scale; }

        const glm::vec3 &GetPosition() const { return position; }
        const glm::vec3 &GetRotation() const { return rotation; }
        float GetScale() const { return scale; }

    protected:
        glm::vec3 position{0};
        glm::vec3 rotation{0};
        float scale{1};
    };

} // cp
