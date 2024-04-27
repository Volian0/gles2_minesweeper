#pragma once

#include <glm/glm.hpp>
#include <stb/stb_image.h>

#include <string_view>

class Image final
{
public:
    Image(std::string_view t_file);
    ~Image() noexcept;

    Image(const Image&) = delete;
    Image(Image&&) = delete;
    Image& operator=(const Image&) = delete;
    Image& operator=(Image&&) = delete;

    [[nodiscard]] constexpr auto ptr() noexcept
    {
        return m_ptr;
    }

    [[nodiscard]] constexpr auto size() const noexcept
    {
        return m_size;
    }

private:
    stbi_uc* m_ptr;
    glm::ivec2 m_size{};
};