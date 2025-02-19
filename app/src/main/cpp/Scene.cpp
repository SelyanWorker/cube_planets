#include <glm/gtx/string_cast.hpp>

#include "Scene.h"
#include "Input.h"
#include "LogBuffer.h"

namespace cp {

    void Scene::Setup(AAssetManager* assetManager) {
        planets.emplace_back(std::make_shared<CubePlanet>(Texture2D::Create(assetManager, "scrap.jpg"), 16, 1.0f, 5.f, true));
        planets.emplace_back(std::make_shared<CubePlanet>(Texture2D::Create(assetManager, "ice_world_like_planet.jpg"), 12, 0.8f, 4.f, false));
        planets.emplace_back(std::make_shared<CubePlanet>(Texture2D::Create(assetManager, "mars_planet.jpg"), 8, 0.6f, 3.f, true));
        planets.emplace_back(std::make_shared<CubePlanet>(Texture2D::Create(assetManager, "tumblr_planet.jpg"), 4, 0.4f, 2.f, false));

        sun = std::make_shared<CubeSun>(Texture2D::Create(assetManager, "sun.jpg"), 0.3f);

        camera.SetPosition(glm::vec3{25, 10, 0});
    }


    void Scene::Update(int64_t elapsedTime) {
        for(const auto& planetPtr : planets) {
            planetPtr->Update(elapsedTime);
        }

        sun->Update(elapsedTime);
    }


    void Scene::Draw() {
        DrawPlanets();
        DrawSun();
    }


    void Scene::DrawPlanets() {
        Shader::WeakPtr cubePlanetShaderWeak = gShaderLib.Get(planetsShaderName);
        if (cubePlanetShaderWeak.expired()) {
            return;
        }

        auto planetShader = cubePlanetShaderWeak.lock();
        planetShader->Activate();
        planetShader->SetUniform("uProjection", camera.GetProjectionMatrix());
        planetShader->SetUniform("uView", camera.GetViewMatrix());
        planetShader->SetUniform("uViewPos", camera.GetPosition());
        planetShader->SetUniform("uLightPos", sun->GetPosition());
        planetShader->SetUniform("uLightAmbientStrength", 0.4f);
        planetShader->SetUniform("uLightSpecularStrength", 0.7f);
        planetShader->SetUniform("uLightColour", glm::vec3{1});

        for(const auto& planetPtr : planets) {
            planetShader->SetUniform("uModel", planetPtr->CreateModelMatrix());
            planetPtr->Draw(*planetShader);
        }

        planetShader->Deactivate();
    }


    void Scene::DrawSun() {
        Shader::WeakPtr sunShaderWeak = gShaderLib.Get(sunShaderName);
        if (sunShaderWeak.expired()) {
            return;
        }

        auto sunShader = sunShaderWeak.lock();
        sunShader->Activate();
        sunShader->SetUniform("uProjection", camera.GetProjectionMatrix());
        sunShader->SetUniform("uView", camera.GetViewMatrix());
        sunShader->SetUniform("uLightAmbientStrength", 0.7f);
        sunShader->SetUniform("uLightColour", glm::vec3{1});

        sunShader->SetUniform("uModel", sun->CreateModelMatrix());
        sun->Draw(*sunShader);

        sunShader->Deactivate();
    }

}
