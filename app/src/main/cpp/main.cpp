#include <jni.h>
#include <android/choreographer.h>
#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>
#include <game-activity/native_app_glue/android_native_app_glue.c>
#include <chrono>

#include "LogBuffer.h"
#include "Engine.h"

extern "C" {

bool MotionEventFilter(const GameActivityMotionEvent *motionEvent) {
    auto sourceClass = motionEvent->source & AINPUT_SOURCE_CLASS_MASK;
    return sourceClass == AINPUT_SOURCE_CLASS_POINTER || sourceClass == AINPUT_SOURCE_CLASS_JOYSTICK;
}


void ScheduleNextTick(void* context) {
    AChoreographer_postFrameCallback64(AChoreographer_getInstance(), [](int64_t time, void* context){
        static std::chrono::duration prevTime = std::chrono::nanoseconds{time};
        const std::chrono::duration curTime = std::chrono::nanoseconds{time};
        ScheduleNextTick(context);
        auto engine = static_cast<cp::Engine*>(context);
        engine->OnFrameTick(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - prevTime).count());
        prevTime = curTime;
    }, context);
}


void InitEngine(android_app *pApp) {
    android_app_set_motion_event_filter(pApp, MotionEventFilter);

    pApp->userData = new cp::Engine(pApp);
    ScheduleNextTick(pApp->userData);
}


void TermEngine(android_app *pApp) {
    if (pApp->userData) {
        auto *pEngine = static_cast<cp::Engine*>(pApp->userData);
        pApp->userData = nullptr;
        delete pEngine;
    }

    android_app_set_motion_event_filter(pApp, nullptr);
}


void HandleCmd(android_app *pApp, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            InitEngine(pApp);
            break;
        case APP_CMD_TERM_WINDOW:
            TermEngine(pApp);
            break;
        default:
            break;
    }
}


void android_main(struct android_app *pApp) {
    pApp->onAppCmd = HandleCmd;

    while (!pApp->destroyRequested) {
        android_poll_source* source = nullptr;
        auto result = ALooper_pollOnce(0, nullptr, nullptr,
                                       reinterpret_cast<void**>(&source));
        if (result == ALOOPER_POLL_ERROR) {
            break;
        }

        if (source != nullptr) {
            source->process(pApp, source);
        }
    }

    TermEngine(pApp);
}

}