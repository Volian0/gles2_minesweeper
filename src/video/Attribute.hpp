#pragma once

#include <cstdint>
#include <string_view>

struct Attribute final
{
    std::string_view name;
    std::uint8_t size;
};