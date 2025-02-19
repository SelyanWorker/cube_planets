#include <game-activity/native_app_glue/android_native_app_glue.h>

#include "Input.h"
#include "LogBuffer.h"

namespace cp {

    class Input::Impl {
    public:
        void HandleInput(android_app *pApp);

        const glm::vec2& GetPointerPosition() const { return pointerPosition; }

    private:
        glm::vec2 pointerPosition{0};
    };


    void Input::Impl::HandleInput(android_app *pApp) {
        if (!pApp) {
            return;
        }

        auto *inputBuffer = android_app_swap_input_buffers(pApp);
        if (!inputBuffer) {
            return;
        }

        for (auto i = 0; i < inputBuffer->motionEventsCount; i++) {
            auto &motionEvent = inputBuffer->motionEvents[i];

            const auto pointerIndex = (motionEvent.action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                    >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

            auto &pointer = motionEvent.pointers[pointerIndex];

            pointerPosition = glm::vec2{
                    GameActivityPointerAxes_getX(&pointer),
                    GameActivityPointerAxes_getY(&pointer)
            };
        }

        android_app_clear_motion_events(inputBuffer);
    }


    Input::Impl* Input::pImpl = nullptr;


    void Input::Init() {
        if (!pImpl) {
            pImpl = new Impl();
        }
    }


    void Input::Term() {
        delete pImpl;
    }


    void Input::HandleInput(android_app *pApp) {
        if (pImpl) {
            pImpl->HandleInput(pApp);
        }
    }


    glm::vec2 Input::GetPointerPosition() {
        if (!pImpl) {
            assert(false);
            return glm::vec2{0};
        }

        return pImpl->GetPointerPosition();
    }

} // cp