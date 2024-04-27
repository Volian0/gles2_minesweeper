#pragma once

#include <SDL_opengles2.h>

class ShaderAttachment final
{
public:
    ShaderAttachment(GLuint t_program_name, GLuint t_shader_name) noexcept;
    ~ShaderAttachment() noexcept;

    ShaderAttachment(const ShaderAttachment&) = delete;
    ShaderAttachment(ShaderAttachment&&) = delete;
    ShaderAttachment& operator=(const ShaderAttachment&) = delete;
    ShaderAttachment& operator=(ShaderAttachment&&) = delete;

private:
    GLuint m_program_name;
    GLuint m_shader_name;
};