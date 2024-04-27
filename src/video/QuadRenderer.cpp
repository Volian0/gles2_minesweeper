#include "QuadRenderer.hpp"

#include <SDL_opengles2.h>

#include <cstdint>

QuadRenderer::QuadRenderer(std::span<const Attribute> t_attributes)
{
    m_vbo_buffer.bind(GL_ARRAY_BUFFER);
    m_ebo_buffer.bind(GL_ELEMENT_ARRAY_BUFFER);
    constexpr std::size_t VBO_SIZE = 40000;
    constexpr std::size_t EBO_SIZE = 6000;
    VboBuffer::data(GL_ARRAY_BUFFER, VBO_SIZE, nullptr, GL_DYNAMIC_DRAW);
    m_vbo_vector.reserve(VBO_SIZE);
    std::vector<GLushort> ebo_vector;
    ebo_vector.reserve(EBO_SIZE);
    for (std::size_t index = 0; index < EBO_SIZE / ELEMENTS_IN_QUAD; ++index)
    {
        const auto offset = index * 4;
        ebo_vector.emplace_back(offset + 2);
        ebo_vector.emplace_back(offset + 1);
        ebo_vector.emplace_back(offset + 0);
        ebo_vector.emplace_back(offset + 3);
        ebo_vector.emplace_back(offset + 2);
        ebo_vector.emplace_back(offset + 0);
    }
    EboBuffer::data(GL_ELEMENT_ARRAY_BUFFER, EBO_SIZE, ebo_vector.data(), GL_STATIC_DRAW);
    enable_attributes(t_attributes);
}

void QuadRenderer::add_quad(const std::array<glm::vec2, 4>& t_positions, const std::array<glm::vec2, 4>& t_uvs,
                            const glm::vec3& t_tint)
{
    for (std::size_t index = 0; index < 4; ++index)
    {
        vbo_add(glm::vec3{t_positions[index], 0.0F});
        vbo_add(t_uvs[index]);
        vbo_add(t_tint);
    }
    m_element_count += ELEMENTS_IN_QUAD;
}

void QuadRenderer::clear() noexcept
{
    glClear(GL_COLOR_BUFFER_BIT);
    m_element_count = 0;
    m_vbo_vector.clear();
}
void QuadRenderer::draw()
{
    VboBuffer::subdata(GL_ARRAY_BUFFER, 0, m_vbo_vector);
    EboBuffer::draw_elements(m_element_count, GL_UNSIGNED_SHORT);
}

void QuadRenderer::enable_attributes(std::span<const Attribute> t_attributes)
{
    std::uint_fast8_t offset{0};
    std::uint_fast8_t stride{0};
    for (std::size_t index = 0; index < t_attributes.size(); ++index)
    {
        stride += t_attributes[index].size;
    }
    for (std::size_t index = 0; index < t_attributes.size(); ++index)
    {
        glEnableVertexAttribArray(index);
        const auto size = t_attributes[index].size;
        decltype(m_vbo_buffer)::vertex_attribute_pointer(index, size, GL_FLOAT, stride, offset);
        offset += size;
    }
}