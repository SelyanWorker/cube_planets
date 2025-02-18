#pragma once

#include <glm/glm.hpp>

struct android_app;

namespace cp {

    class Input {
    public:
        static void Init();
        static void Term();

        static void HandleInput(android_app *pApp);
        static glm::vec2 GetPointerPosition();

    private:
        class Impl;
        static Impl* pImpl;
    };

} // cp
