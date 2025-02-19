#include "DrawableTextured.h"

namespace cp {

    void DrawableTextured::Draw(const Shader &shader) const {
        TextureBindHelper bindHelper{texture};

        DrawableBase::Draw(shader);
    }

} // cp