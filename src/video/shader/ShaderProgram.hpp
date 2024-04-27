#pragma once

#include "../Attribute.hpp"

#include <SDL_opengles2.h>

#include <span>
#include <string_view>

class ShaderProgram
{
public:
    ShaderProgram();
    virtual ~ShaderProgram() noexcept;

    void bind_attributes_locations(std::span<const Attribute> t_attributes) noexcept;
    [[nodiscard]] GLint get_uniform_location(std::string_view t_uniform_name) const;
    void link(std::string_view t_file_vertex, std::string_view t_file_fragment);
    void use() const noexcept;

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram& operator=(ShaderProgram&&) = delete;

private:
    GLuint m_name;
};