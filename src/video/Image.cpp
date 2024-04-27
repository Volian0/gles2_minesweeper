#include "Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <stdexcept>
#include <string_view>

Image::Image(std::string_view t_file)
{
    int channels{};
    m_ptr = stbi_load(t_file.data(), &m_size.x, &m_size.y, &channels, 4);
    if (m_ptr == nullptr)
    {
        throw std::runtime_error("Failed to open image!");
    }
}
Image::~Image() noexcept
{
    stbi_image_free(m_ptr);
}