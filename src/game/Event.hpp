#pragma once

#include <glm/glm.hpp>

#include <cstdint>

struct Event final
{
    enum class Type : std::uint8_t
    {
        MOUSE_DOWN,
        KEYBOARD_DOWN,
        WINDOW_RESIZE,
    } type;
    enum class Button : std::uint8_t
    {
        NONE,
        LEFT,
        RIGHT
    } button{Button::NONE};
    enum class Key : std::uint8_t
    {
        NONE,
        ESC,
        R
    } key{Key::NONE};
    glm::uvec2 coordinates{};
};