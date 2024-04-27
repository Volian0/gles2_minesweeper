#include "GameState.hpp"

#include "Game.hpp"

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/transform.hpp>

void GameState::update_matrix(const glm::vec2& t_offset, float t_roll_angle)
{
    const auto aspect_ratio = m_camera_center.x / m_camera_center.y;
    m_camera_z = -m_camera_center.y;
    if (m_game->get_aspect_ratio() < aspect_ratio)
    {
        m_camera_z *= (aspect_ratio / m_game->get_aspect_ratio());
    }
    const auto view_matrix = glm::lookAt(
        glm::vec3{m_camera_center, m_camera_z},
        glm::vec3{
            m_camera_center + t_offset,
            0.0F,
        },
        glm::vec3(glm::rotate(t_roll_angle, glm::vec3{0.0F, 0.0F, 1.0F}) * glm::vec4{0.0F, -1.0F, 0.0F, 1.0F}));
    const auto matrix = m_game->get_projection_matrix() * view_matrix;
    m_game->update_matrix(matrix);
}

bool GameState::does_button_collide(const glm::vec2& t_button_position, const glm::vec2& t_button_size,
                                    const glm::vec2& t_mouse_position) const noexcept
{
    auto mouse_f = glm::vec2{t_mouse_position} / glm::vec2{m_game->get_window_size()};
    const auto scale = glm::vec2{1.0F / -m_camera_z * 0.5F} / glm::vec2{m_game->get_aspect_ratio(), 1.0F};
    const glm::vec2 button_position = t_button_position * scale;
    const glm::vec2 button_size = t_button_size * scale;
    const glm::vec2 starting_position{0.5F - float(m_camera_center.x / -m_camera_z * 0.5F / m_game->get_aspect_ratio()),
                                      0.5F - float(m_camera_center.y) / -m_camera_z * 0.5F};
    mouse_f -= starting_position;
    return (mouse_f.x > button_position.x && mouse_f.x < button_position.x + button_size.x &&
            mouse_f.y > button_position.y && mouse_f.y < button_position.y + button_size.y);
}