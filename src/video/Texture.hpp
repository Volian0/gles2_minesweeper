#pragma once

#include <SDL_opengles2.h>

#include <string_view>

class Texture final
{
public:
    Texture();

    ~Texture() noexcept;

    static void image(std::string_view t_file);

    void bind() const noexcept;

    Texture(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;

private:
    GLuint m_name{0};
};