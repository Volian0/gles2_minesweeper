#pragma once

#include <SDL_opengles2.h>

#include <string_view>

class Shader final
{
public:
    Shader(GLenum t_type);
    ~Shader() noexcept;

    void compile(std::string_view t_file);

    [[nodiscard]] constexpr auto name() const noexcept
    {
        return m_name;
    }

    Shader(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&) = delete;

private:
    GLuint m_name;
};