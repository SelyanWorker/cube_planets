#pragma once

#include <EGL/egl.h>
#include <memory>

#include "Shader.h"
#include "Camera.h"
#include "DrawableBase.h"
#include "Scene.h"

struct android_app;

namespace cp {

    class Engine {
    public:
        explicit Engine(android_app *pApp);
        virtual ~Engine();

        void OnFrameTick(int64_t elapsedTime);

    private:
        void InitGraphics();
        void TermGraphics();

        void UpdateRenderArea();

        void Render();

    private:
        android_app *pApp;
        EGLDisplay display_;
        EGLSurface surface_;
        EGLContext context_;
        EGLint width_;
        EGLint height_;

        bool shaderNeedsNewProjectionMatrix_;

        std::unique_ptr<Shader> cubePlanetShader;

        cp::Scene scene;
    };

}