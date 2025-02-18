#include <memory>
#include <vector>

#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <android/imagedecoder.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Engine.h"

#include "AndroidOut.h"
#include "Shader.h"
#include "Utility.h"
#include "Texture2D.h"
#include "Input.h"

//! executes glGetString and outputs the result to logcat
#define PRINT_GL_STRING(s) {aout << #s": "<< glGetString(s) << std::endl;}

/*!
 * @brief if glGetString returns a space separated list of elements, prints each one on a new line
 *
 * This works by creating an istringstream of the input c-style string. Then that is used to create
 * a vector -- each element of the vector is a new element in the input string. Finally a foreach
 * loop consumes this and outputs it to logcat using @a aout
 */
#define PRINT_GL_STRING_AS_LIST(s) { \
std::istringstream extensionStream((const char *) glGetString(s));\
std::vector<std::string> extensionList(\
        std::istream_iterator<std::string>{extensionStream},\
        std::istream_iterator<std::string>());\
aout << #s":\n";\
for (auto& extension: extensionList) {\
    aout << extension << "\n";\
}\
aout << std::endl;\
}

//! Color for cornflower blue. Can be sent directly to glClearColor
#define CORNFLOWER_BLUE 100 / 255.f, 149 / 255.f, 237 / 255.f, 1

// Vertex shader, you'd typically load this from assets
static const char *vertex = R"vertex(#version 300 es

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

// Fragment shader, you'd typically load this from assets
static const char *fragment = R"fragment(#version 300 es

precision mediump float;

in vec2 fragUV;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D uTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 outColor;

void main() {
    vec4 textureColor = texture(uTexture, fragUV);

    vec3 lightColor = vec3(1, 1, 1);
    float ambientStrength = 0.25;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    outColor = textureColor * vec4(ambient + diffuse + specular, 1);
}
)fragment";

// Vertex shader, you'd typically load this from assets
static const char *vertexBloom = R"vertex(#version 300 es
in vec3 inPosition;
in vec2 inUV;
in vec3 inNormal;

uniform mat4 uProjection;
uniform mat4 uModel;
uniform mat4 uView;

out vec2 fragUV;

void main() {
    fragUV = inUV;
    gl_Position = uProjection * uView * uModel * vec4(inPosition, 1.0);
}
)vertex";

// Fragment shader, you'd typically load this from assets
static const char *fragmentBloom = R"fragment(#version 300 es

in vec2 fragUV;

out vec4 pixel;

uniform sampler2D uTexture;
uniform float bloom_spread = 1;
uniform float bloom_intensity = 2;

void main() {
    ivec2 size = textureSize(uTexture, 0);

    float uv_x = fragUV.x * size.x;
    float uv_y = fragUV.y * size.y;

    vec4 sum = vec4(0.0);
    for (int n = 0; n < 9; ++n) {
        uv_y = (fragUV.y * size.y) + (bloom_spread * float(n - 4));
        vec4 h_sum = vec4(0.0);
        h_sum += texelFetch(uTexture, ivec2(uv_x - (4.0 * bloom_spread), uv_y), 0);
        h_sum += texelFetch(uTexture, ivec2(uv_x - (3.0 * bloom_spread), uv_y), 0);
        h_sum += texelFetch(uTexture, ivec2(uv_x - (2.0 * bloom_spread), uv_y), 0);
        h_sum += texelFetch(uTexture, ivec2(uv_x - bloom_spread, uv_y), 0);
        h_sum += texelFetch(uTexture, ivec2(uv_x, uv_y), 0);
        h_sum += texelFetch(uTexture, ivec2(uv_x + bloom_spread, uv_y), 0);
        h_sum += texelFetch(uTexture, ivec2(uv_x + (2.0 * bloom_spread), uv_y), 0);
        h_sum += texelFetch(uTexture, ivec2(uv_x + (3.0 * bloom_spread), uv_y), 0);
        h_sum += texelFetch(uTexture, ivec2(uv_x + (4.0 * bloom_spread), uv_y), 0);
        sum += h_sum / 9.0;
    }

    pixel = texture(uTexture, fragUV) - ((sum / 9.0) * bloom_intensity);
}
)fragment";


cp::Engine::Engine(android_app *pApp)
: pApp(pApp),
  display_(EGL_NO_DISPLAY),
  surface_(EGL_NO_SURFACE),
  context_(EGL_NO_CONTEXT),
  width_(0),
  height_(0),
  shaderNeedsNewProjectionMatrix_(true) {
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
    // Choose your render attributes
    constexpr EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_NONE
    };

    // The default display is probably what you want on Android
    auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, nullptr, nullptr);

    // figure out how many configs there are
    EGLint numConfigs;
    eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);

    // get the list of configurations
    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
    eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);

    // Find a config we like.
    // Could likely just grab the first if we don't care about anything else in the config.
    // Otherwise hook in your own heuristic
    auto config = *std::find_if(
            supportedConfigs.get(),
            supportedConfigs.get() + numConfigs,
            [&display](const EGLConfig &config) {
                EGLint red, green, blue, depth;
                if (eglGetConfigAttrib(display, config, EGL_RED_SIZE, &red)
                    && eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &green)
                    && eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &blue)
                    && eglGetConfigAttrib(display, config, EGL_DEPTH_SIZE, &depth)) {

                    aout << "Found config with " << red << ", " << green << ", " << blue << ", "
                         << depth << std::endl;
                    return red == 8 && green == 8 && blue == 8 && depth == 24;
                }
                return false;
            });

    aout << "Found " << numConfigs << " configs" << std::endl;
    aout << "Chose " << config << std::endl;

    // create the proper window surface
    EGLint format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    EGLSurface surface = eglCreateWindowSurface(display, config, pApp->window, nullptr);

    // Create a GLES 3 context
    EGLint contextAttribs[] = {EGL_CONTEXT_MAJOR_VERSION, 3, EGL_NONE};
    EGLContext context = eglCreateContext(display, config, nullptr, contextAttribs);

    // get some window metrics
    auto madeCurrent = eglMakeCurrent(display, surface, surface, context);
    assert(madeCurrent);

    display_ = display;
    surface_ = surface;
    context_ = context;

    // make width and height invalid so it gets updated the first frame in @a updateRenderArea()
    width_ = -1;
    height_ = -1;

    PRINT_GL_STRING(GL_VENDOR);
    PRINT_GL_STRING(GL_RENDERER);
    PRINT_GL_STRING(GL_VERSION);
    PRINT_GL_STRING_AS_LIST(GL_EXTENSIONS);

    // setup any other gl related global states
    glClearColor(CORNFLOWER_BLUE);

    // enable alpha globally for now, you probably don't want to do this in a game
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    cubePlanetShader = std::unique_ptr<Shader>(
            Shader::Create(vertex, fragment, "inPosition", "inUV", "inNormal"));
    assert(cubePlanetShader);

    cubePlanetShader->Activate();
}


void cp::Engine::TermGraphics() {
    if (display_ == EGL_NO_DISPLAY) {
       return;
    }

    eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (context_ != EGL_NO_CONTEXT) {
        eglDestroyContext(display_, context_);
    }
    if (surface_ != EGL_NO_SURFACE) {
        eglDestroySurface(display_, surface_);
    }
    eglTerminate(display_);
}


void cp::Engine::UpdateRenderArea() {
    EGLint width;
    eglQuerySurface(display_, surface_, EGL_WIDTH, &width);

    EGLint height;
    eglQuerySurface(display_, surface_, EGL_HEIGHT, &height);

    if (width != width_ || height != height_) {
        width_ = width;
        height_ = height;
        glViewport(0, 0, width, height);

        // make sure that we lazily recreate the projection matrix before we render
        shaderNeedsNewProjectionMatrix_ = true;
    }
}


void cp::Engine::Render() {
    UpdateRenderArea();

    if (shaderNeedsNewProjectionMatrix_) {
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.f), float(width_) / height_, 0.1f, 100.f);
        cubePlanetShader->SetUniform("uProjection", projectionMatrix);

        shaderNeedsNewProjectionMatrix_ = false;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene.Draw(*cubePlanetShader);

    auto swapResult = eglSwapBuffers(display_, surface_);
    assert(swapResult == EGL_TRUE);
}
