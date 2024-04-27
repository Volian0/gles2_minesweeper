#pragma once

#include "../video/Sprite.hpp"
#include "Button.hpp"
#include "GameState.hpp"
#include "Minesweeper.hpp"

#include <glm/glm.hpp>

#include <optional>
#include <random>
#include <vector>

class StateLevel final : public GameState
{
public:
    enum class Buttons : std::uint_fast8_t
    {
        EXIT,
        RESET
    };

    static constexpr Sprite COVERED_CELL{{0, 5}, {9, 9}};
    static constexpr Sprite FLAGGED_CELL{{0, 14}, {9, 9}};
    static constexpr Sprite WRONGLY_FLAGGED_CELL{{9, 14}, {9, 9}};
    static constexpr Sprite MINE_CELL{{0, 23}, {9, 9}};
    static constexpr Sprite TRIGGERED_MINE_CELL{{9, 23}, {9, 9}};
    static constexpr Sprite UNCOVERED_CELL{{9, 5}, {9, 9}};
    static constexpr Sprite CELL_TARGET{{18, 23}, {9, 9}};
    static constexpr Sprite FLAG_ICON{{18, 5}, {5, 5}};

    StateLevel(class Game* t_game, glm::uvec2 t_minesweeper_size, unsigned t_bombs_number,
               unsigned t_level_number) noexcept;

    static constexpr float GRAVITY_ACCELERATION = 50.0F;

    void update();

private:
    void handle_events();
    void draw_timer();
    void draw_flags();
    void draw_cells();
    void draw_particles();
    void draw_buttons();
    void handle_shake();
    void draw_cell_target();
    void reset();
    void exit();
    void handle_button_click(const glm::uvec2& t_mouse_position);
    void left_click(const glm::uvec2& t_mouse_cell);
    void right_click(const glm::uvec2& t_mouse_cell);

    struct ParticleCell final
    {
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec2 acceleration;
        float accumulated_time{0.0F};
        constexpr void update(float t_delta_time) noexcept
        {
            t_delta_time *= 1.0F;
            velocity += acceleration * t_delta_time * 0.5F;
            position += velocity * t_delta_time;
            velocity += acceleration * t_delta_time * 0.5F;
            accumulated_time += t_delta_time;
        }
    };

    [[nodiscard]] std::optional<glm::uvec2> get_mouse_cell(const glm::uvec2& t_mouse_position) const noexcept;

    Minesweeper m_minesweeper;
    std::optional<glm::uvec2> m_target_cell;
    std::vector<ParticleCell> m_particles;
    std::optional<std::chrono::time_point<std::chrono::steady_clock>> m_tp_explosion;
    std::uniform_real_distribution<float> m_rng_distribution{-1.0F, 1.0F};
    std::optional<Buttons> m_clicked_button;

    float m_layout_y_top;
    float m_layout_y_bottom;
    float m_layout_x_icon;
    float m_layout_x_numbers;
    glm::vec2 m_layout_cells;
    float m_layout_button_offset;
    ButtonLayout m_button_layout;
    unsigned m_level_number;
};