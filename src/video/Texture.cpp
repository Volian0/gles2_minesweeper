#include "Texture.hpp"

#include "Image.hpp"

#include <stdexcept>

Texture::Texture()
{
    glGenTextures(1, &m_name);
    if (m_name == 0)
    {
        throw std::runtime_error("Failed to generate texture!");
    }
}

Texture::~Texture() noexcept
{
    glDeleteTextures(1, &m_name);
}

void Texture::image(std::string_view t_file)
{
    Image image{t_file};
    const auto image_size = image.size();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_size.x, image_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.ptr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::bind() const noexcept
{
    glBindTexture(GL_TEXTURE_2D, m_name);
}