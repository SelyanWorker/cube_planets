#include <glm/gtx/string_cast.hpp>

#include "Scene.h"
#include "Input.h"
#include "AndroidOut.h"

void cp::Scene::Setup(AAssetManager* assetManager) {
    auto scrapTexture = Texture2D::Create(assetManager, "scrap.jpg", 0);
    planets.emplace_back(std::make_shared<CubePlanet>(scrapTexture));

    camera.GetPosition() = glm::vec3{-8, 4, 0};
}

void cp::Scene::Update(int64_t elapsedTime) {
    for(const auto& planetPtr : planets) {
        planetPtr->Update(elapsedTime);
    }
}

void cp::Scene::Draw(const Shader &shader) {
    shader.SetUniform("uView", camera.CreteViewMatrix());
    shader.SetUniform("viewPos", camera.GetPosition());
    shader.SetUniform("lightPos", lightPosition);

    for(const auto& planetPtr : planets) {
        planetPtr->Draw(shader);
    }
}
