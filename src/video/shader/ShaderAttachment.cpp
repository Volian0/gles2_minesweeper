#include "ShaderAttachment.hpp"

ShaderAttachment::ShaderAttachment(GLuint t_program_name, GLuint t_shader_name) noexcept
    : m_program_name{t_program_name}, m_shader_name{t_shader_name}
{
    glAttachShader(m_program_name, m_shader_name);
}
ShaderAttachment::~ShaderAttachment() noexcept
{
    glDetachShader(m_program_name, m_shader_name);
}