#pragma once

#include <glm/glm.hpp>

#include <cstdint>

class GameState
{
public:
    enum class Type : std::uint_fast8_t
    {
        MAIN_MENU,
        LEVEL_SELECT,
        LEVEL,
        CREDITS
    };
    constexpr GameState(Type t_type, class Game* t_game, const glm::vec2& t_camera_center = glm::vec2{1.0F}) noexcept
        : m_game{t_game}, m_type{t_type}, m_camera_center{t_camera_center}
    {
    }
    constexpr virtual ~GameState() noexcept = default;
    GameState(const GameState&) = delete;
    GameState(GameState&&) = delete;
    GameState& operator=(const GameState&) = delete;
    GameState& operator=(GameState&&) = delete;

    [[nodiscard]] constexpr Type get_type() const noexcept
    {
        return m_type;
    }

    void update_matrix(const glm::vec2& t_offset = glm::vec2{0.0F}, float t_roll_angle = 0.0F);

    bool does_button_collide(const glm::vec2& t_button_position, const glm::vec2& t_button_size,
                             const glm::vec2& t_mouse_position) const noexcept;

    [[nodiscard]] constexpr class Game& game() noexcept
    {
        return *m_game;
    }

    [[nodiscard]] constexpr const class Game& game() const noexcept
    {
        return *m_game;
    }

    [[nodiscard]] constexpr auto get_camera_z() const noexcept
    {
        return m_camera_z;
    }

    constexpr void set_camera_center(const glm::vec2& t_camera_center) noexcept
    {
        m_camera_center = t_camera_center;
    }

private:
    class Game* m_game;
    Type m_type;
    glm::vec2 m_camera_center;
    float m_camera_z{0.0F};
};