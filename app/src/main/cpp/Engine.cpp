#include <memory>
#include <vector>

#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <android/imagedecoder.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Engine.h"

#include "LogBuffer.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Input.h"

#define CORNFLOWER_BLUE 100 / 255.f, 149 / 255.f, 237 / 255.f, 1

namespace {

    const char* planetShaderVertex = R"vertex(#version 300 es
    in vec3 inPosition;
    in vec2 inUV;
    in vec3 inNormal;

    uniform mat4 uProjection;
    uniform mat4 uModel;
    uniform mat4 uView;

    out vec2 fragUV;
    out vec3 FragPos;
    out vec3 Normal;

    void main() {
        gl_Position = uProjection * uView * uModel * vec4(inPosition, 1.0);

        fragUV = inUV;
        Normal = mat3(transpose(inverse(uModel))) * inNormal;
        FragPos = vec3(uModel * vec4(inPosition, 1.0));
    }
    )vertex";

    static const char* planetShaderFragment = R"fragment(#version 300 es
    precision mediump float;

    in vec2 fragUV;
    in vec3 FragPos;
    in vec3 Normal;

    uniform sampler2D uTexture;
    uniform vec3 uLightPos;
    uniform vec3 uViewPos;
    uniform vec3 uLightColour;
    uniform float uLightAmbientStrength;
    uniform float uLightSpecularStrength;

    out vec4 outColor;

    void main() {
        vec4 textureColor = texture(uTexture, fragUV);

        vec3 ambient = uLightAmbientStrength * uLightColour;

        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(uLightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * uLightColour;

        vec3 viewDir = normalize(uViewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3 specular = uLightSpecularStrength * spec * uLightColour;

        outColor = textureColor * vec4(ambient + diffuse + specular, 1);
    }
    )fragment";

    const char* sunShaderVertex = R"vertex(#version 300 es
    in vec3 inPosition;
    in vec2 inUV;
    in vec3 inNormal;

    uniform mat4 uProjection;
    uniform mat4 uModel;
    uniform mat4 uView;

    out vec2 fragUV;

    void main() {
        gl_Position = uProjection * uView * uModel * vec4(inPosition, 1.0);
        fragUV = inUV;
    }
    )vertex";

    const char* sunShaderFragment = R"fragment(#version 300 es
    precision mediump float;

    in vec2 fragUV;

    out vec4 outColor;

    uniform sampler2D uTexture;
    uniform float uLightAmbientStrength;
    uniform vec3 uLightColour;

    void main() {
        outColor = texture(uTexture, fragUV) + vec4(uLightColour * uLightAmbientStrength, 0);
    }
    )fragment";

    constexpr const char* PLANET_SHADER_NAME = "PlanetShader";
    constexpr const char* SUN_SHADER_NAME = "SunShader";

}

cp::Engine::Engine(android_app *pApp)
: pApp(pApp)
, scene(PLANET_SHADER_NAME, SUN_SHADER_NAME){
    InitGraphics();
    Input::Init();
    scene.Setup(pApp->activity->assetManager);
}


cp::Engine::~Engine() {
    cp::Input::Term();
    TermGraphics();
}


void cp::Engine::OnFrameTick(int64_t elapsedTime) {
    Input::HandleInput(pApp);

    scene.Update(elapsedTime);

    Render();
}


void cp::Engine::InitGraphics() {
    constexpr EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_NONE
    };

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, nullptr, nullptr);

    EGLint numConfigs;
    eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);

    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
    eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);

    auto config = *std::find_if(
            supportedConfigs.get(),
            supportedConfigs.get() + numConfigs,
            [display = display](const EGLConfig &config) {
                EGLint red, green, blue, depth;
                if (eglGetConfigAttrib(display, config, EGL_RED_SIZE, &red)
                    && eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &green)
                    && eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &blue)
                    && eglGetConfigAttrib(display, config, EGL_DEPTH_SIZE, &depth)) {

                    cp::out << "Found config with " << red << ", " << green << ", " << blue << ", "
                         << depth << std::endl;
                    return red == 8 && green == 8 && blue == 8 && depth == 24;
                }
                return false;
            });

    cp::out << "Found " << numConfigs << " configs" << std::endl;
    cp::out << "Chose " << config << std::endl;

    EGLint format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    surface = eglCreateWindowSurface(display, config, pApp->window, nullptr);

    EGLint contextAttribs[] = {EGL_CONTEXT_MAJOR_VERSION, 3, EGL_NONE};
    context = eglCreateContext(display, config, nullptr, contextAttribs);

    auto madeCurrent = eglMakeCurrent(display, surface, surface, context);
    assert(madeCurrent);

    glClearColor(CORNFLOWER_BLUE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    Shader::Ptr planetShader = Shader::Create(planetShaderVertex, planetShaderFragment,
                                      "inPosition", "inUV", "inNormal");
    if (planetShader) {
        gShaderLib.Add(PLANET_SHADER_NAME, planetShader);
    } else {
        out << "Failed to create " << PLANET_SHADER_NAME << " shader" << std::endl;
    }

    Shader::Ptr sunShader = Shader::Create(sunShaderVertex, sunShaderFragment,
                                           "inPosition", "inUV", "inNormal");
    if (sunShader) {
        gShaderLib.Add(SUN_SHADER_NAME, sunShader);
    } else {
        out << "Failed to create " << SUN_SHADER_NAME << " shader" << std::endl;
    }
}


void cp::Engine::TermGraphics() {
    gShaderLib.Clear();

    if (display == EGL_NO_DISPLAY) {
       return;
    }

    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (context != EGL_NO_CONTEXT) {
        eglDestroyContext(display, context);
    }
    if (surface != EGL_NO_SURFACE) {
        eglDestroySurface(display, surface);
    }
    eglTerminate(display);
}


void cp::Engine::UpdateRenderArea() {
    EGLint prevWidth;
    eglQuerySurface(display, surface, EGL_WIDTH, &prevWidth);

    EGLint prevHeight;
    eglQuerySurface(display, surface, EGL_HEIGHT, &prevHeight);

    if (prevWidth != width || prevHeight != height) {
        width = prevWidth;
        height = prevHeight;
        glViewport(0, 0, width, height);

        shaderNeedsNewProjectionMatrix = true;
    }
}


void cp::Engine::Render() {
    UpdateRenderArea();

    if (shaderNeedsNewProjectionMatrix) {
        scene.SetCameraAspectRation(float(width) / height);
        shaderNeedsNewProjectionMatrix = false;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene.Draw();

    auto swapResult = eglSwapBuffers(display, surface);
    assert(swapResult == EGL_TRUE);
}
