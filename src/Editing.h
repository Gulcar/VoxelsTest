#pragma once

#include "Physics.h"

namespace voxr
{
    void HandleVoxelEditing(voxr::Physics::HitResult& hit, float deltaTime);
    void SetIgnoreClickTime(float time);
}
