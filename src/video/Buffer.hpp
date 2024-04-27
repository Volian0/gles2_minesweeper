#pragma once

#include <SDL_opengles2.h>

#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>

template <typename T> class Buffer final
{
public:
    Buffer()
    {
        glGenBuffers(1, &m_name);
        if (m_name == 0)
        {
            throw std::runtime_error("Failed to generate buffer!");
        }
    }

    ~Buffer() noexcept
    {
        glDeleteBuffers(1, &m_name);
    }

    void bind(GLenum t_target) noexcept
    {
        glBindBuffer(t_target, m_name);
    }

    static void data(GLenum t_target, std::size_t t_size, const T* t_data, GLenum t_usage) noexcept
    {
        glBufferData(t_target, t_size * sizeof(T), t_data, t_usage);
    }

    static void subdata(GLenum t_target, std::size_t t_offset, std::span<const T> t_data) noexcept
    {
        glBufferSubData(t_target, t_offset * sizeof(T), t_data.size() * sizeof(T), t_data.data());
    }

    static void vertex_attribute_pointer(std::uint_fast8_t t_index, std::uint_fast8_t t_size, GLenum t_type,
                                         std::size_t t_stride, std::size_t t_offset) noexcept
    {
        glVertexAttribPointer(t_index, t_size, t_type, GL_FALSE, t_stride * sizeof(T), (void*)(t_offset * sizeof(T)));
    }

    static void draw_elements(std::size_t t_count, GLenum t_type, std::size_t t_offset = 0)
    {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(t_count), t_type, (void*)(t_offset * sizeof(T)));
    }

    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;

private:
    GLuint m_name{0};
};