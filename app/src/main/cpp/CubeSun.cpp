#include "CubeSun.h"

namespace cp {

    void CubeSun::Update(int64_t elapsedTime) {
        rotation.y += spinSpeed;
    }

} // cp