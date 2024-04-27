#include "ShaderProgram.hpp"

#include "Shader.hpp"
#include "ShaderAttachment.hpp"

#include <stdexcept>

ShaderProgram::ShaderProgram() : m_name{glCreateProgram()}
{
    if (m_name == 0)
    {
        throw std::runtime_error("Failed to create shader program!");
    }
}
ShaderProgram::~ShaderProgram() noexcept
{
    glDeleteProgram(m_name);
}
void ShaderProgram::bind_attributes_locations(std::span<const Attribute> t_attributes) noexcept
{
    for (std::size_t index = 0; index < t_attributes.size(); ++index)
    {
        glBindAttribLocation(m_name, index, t_attributes[index].name.data());
    }
}
[[nodiscard]] GLint ShaderProgram::get_uniform_location(std::string_view t_uniform_name) const
{
    const auto location = glGetUniformLocation(m_name, t_uniform_name.data());
    if (location == -1)
    {
        throw std::runtime_error("Failed to get uniform location!");
    }
    return location;
}
void ShaderProgram::link(std::string_view t_file_vertex, std::string_view t_file_fragment)
{
    Shader vertex_shader{GL_VERTEX_SHADER};
    Shader fragment_shader{GL_FRAGMENT_SHADER};

    vertex_shader.compile(t_file_vertex);
    fragment_shader.compile(t_file_fragment);

    ShaderAttachment vertex_shader_attachment{m_name, vertex_shader.name()};
    ShaderAttachment fragment_shader_attachment{m_name, fragment_shader.name()};

    glLinkProgram(m_name);
    GLint success{};
    glGetProgramiv(m_name, GL_LINK_STATUS, &success);
    if (success != GL_TRUE)
    {
        throw std::runtime_error("Failed to link shader program!");
    }
}
void ShaderProgram::use() const noexcept
{
    glUseProgram(m_name);
}