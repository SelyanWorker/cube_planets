#pragma once

#include <utility>
#include <vector>

#include "CubePlanet.h"
#include "CubeSun.h"
#include "Camera.h"

namespace cp {

    class Scene {
    public:
        Scene(std::string planetsShaderName, std::string sunShaderName)
                : camera(30.f, 2, 0.1f, 100.f)
                , planetsShaderName(std::move(planetsShaderName))
                , sunShaderName(std::move(sunShaderName)) { }

        void Setup(AAssetManager* assetManager);
        void Update(int64_t elapsedTime);
        void Draw();

        void SetCameraAspectRation(float aspectRation) {
            camera.SetAspectRation(aspectRation);
        }

    private:
        void DrawPlanets();
        void DrawSun();

    private:
        const std::string planetsShaderName;
        const std::string sunShaderName;

        Camera camera;
        std::vector<std::shared_ptr<CubePlanet>> planets;
        std::shared_ptr<CubeSun> sun;
    };

}
