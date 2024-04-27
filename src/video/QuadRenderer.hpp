#pragma once

#include "Attribute.hpp"
#include "Buffer.hpp"

#include <glm/glm.hpp>

#include <array>
#include <cstddef>
#include <span>
#include <vector>

class QuadRenderer final
{
public:
    static constexpr std::size_t ELEMENTS_IN_QUAD = 6;

    using VboBuffer = Buffer<GLfloat>;
    using EboBuffer = Buffer<GLushort>;

    QuadRenderer(std::span<const Attribute> t_attributes);

    void add_quad(const std::array<glm::vec2, 4>& t_positions, const std::array<glm::vec2, 4>& t_uvs,
                  const glm::vec3& t_tint = glm::vec3{1.0F});

    void clear() noexcept;
    void draw();

private:
    template <typename T> void vbo_add(const T& t_vec)
    {
        for (std::size_t index = 0; index < t_vec.length(); ++index)
        {
            m_vbo_vector.emplace_back(t_vec[index]);
        }
    }

    void enable_attributes(std::span<const Attribute> t_attributes);

    std::size_t m_element_count{};
    std::vector<GLfloat> m_vbo_vector;
    VboBuffer m_vbo_buffer{};
    EboBuffer m_ebo_buffer{};
};