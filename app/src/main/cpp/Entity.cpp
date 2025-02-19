#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Entity.h"

namespace cp {

    glm::mat4 Entity::CreateModelMatrix() const {
        glm::mat4 modelMatrix = glm::translate(glm::mat4{1}, position);
        modelMatrix = glm::rotate(modelMatrix,glm::radians(rotation.x), glm::vec3{1, 0, 0});
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3{0, 1, 0});
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3{0, 0, 1});
        modelMatrix = glm::scale(modelMatrix, glm::vec3{scale});
        return modelMatrix;
    }

}

