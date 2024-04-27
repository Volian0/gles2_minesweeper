#pragma once

#include <glm/glm.hpp>

struct Sprite final
{
    glm::uvec2 position;
    glm::uvec2 size;
    unsigned scale{1};
    unsigned offset{0};
};