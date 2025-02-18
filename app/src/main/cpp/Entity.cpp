#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Entity.h"

glm::mat4 cp::Entity::CreateModelMatrix() const {
    glm::mat4 translationMatrix = glm::translate(glm::identity<glm::mat4>(), position);

    glm::mat4 rotationMatrix = glm::rotate(glm::identity<glm::mat4>(),
                                           glm::radians(rotation.x), glm::vec3{1, 0, 0});
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y),
                                 glm::vec3{0, 1, 0});
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z),
                                 glm::vec3{0, 0, 1});

    glm::mat4 scaleMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3{scale});

    return translationMatrix * rotationMatrix * scaleMatrix;
}
