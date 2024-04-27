#include "Shader.hpp"

#include "../../common/FileToString.hpp"

#include <stdexcept>

Shader::Shader(GLenum t_type) : m_name{glCreateShader(t_type)}
{
    if (m_name == 0)
    {
        throw std::runtime_error("Failed to create shader!");
    }
}
Shader::~Shader() noexcept
{
    glDeleteShader(m_name);
}
void Shader::compile(std::string_view t_file)
{
    const auto source{file_to_string(t_file)};
    const auto* const source_cstr{source.data()};
    const GLint length = static_cast<GLint>(source.size());
    glShaderSource(m_name, 1, &source_cstr, &length);
    glCompileShader(m_name);
    GLint success{};
    glGetShaderiv(m_name, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    {
        throw std::runtime_error("Failed to compile shader!");
    }
}