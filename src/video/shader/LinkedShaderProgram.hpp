#pragma once

#include "../Attribute.hpp"
#include "ShaderProgram.hpp"

#include <span>
#include <string_view>

class LinkedShaderProgram final : private ShaderProgram
{
public:
    LinkedShaderProgram(std::span<const Attribute> t_attributes, std::string_view t_file_vertex,
                        std::string_view t_file_fragment);

    using ShaderProgram::get_uniform_location;
    using ShaderProgram::use;
};