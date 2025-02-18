#pragma once

#include <vector>

#include "CubePlanet.h"
#include "Camera.h"

namespace cp {

    class Scene {
    public:
        void Setup(AAssetManager* assetManager);
        void Update(int64_t elapsedTime);
        void Draw(const Shader& shader);

    private:
        Camera camera;
        std::vector<std::shared_ptr<DrawableBase>> planets;

        glm::vec3 lightPosition{0};
    };

}
