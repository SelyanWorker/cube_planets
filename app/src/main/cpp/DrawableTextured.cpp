#include "DrawableTextured.h"

namespace cp {

    void DrawableTextured::Draw(const Shader &shader) const {
        texture->Bind();

        DrawableBase::Draw(shader);

        texture->Unbind();
    }

} // cp