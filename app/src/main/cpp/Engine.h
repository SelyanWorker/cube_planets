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
        EGLDisplay display{EGL_NO_DISPLAY};
        EGLSurface surface{EGL_NO_SURFACE};
        EGLContext context{EGL_NO_CONTEXT};
        EGLint width{0};
        EGLint height{0};

        bool shaderNeedsNewProjectionMatrix{true};

        cp::Scene scene;
    };

}