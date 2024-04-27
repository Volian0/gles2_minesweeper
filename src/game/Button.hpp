#pragma once

#include "../video/Sprite.hpp"
#include "Game.hpp"
#include "GameState.hpp"

#include <glm/glm.hpp>

#include <optional>
#include <utility>
#include <vector>

class Button final
{
public:
    constexpr Button(const glm::vec2& t_position, const Sprite& t_text, const glm::vec3& t_text_tint = Game::COLOR_TEXT,
                     bool t_big = false, const std::optional<const Sprite*>& t_icon = std::nullopt) noexcept
        : m_position{t_position}, m_text{&t_text}, m_text_tint{t_text_tint}, m_big{t_big}, m_icon{t_icon}
    {
    }

    [[nodiscard]] constexpr const glm::vec2& get_position() const noexcept
    {
        return m_position;
    }

    [[nodiscard]] constexpr glm::vec2 get_size() const noexcept
    {
        return m_big ? Game::BIG_BUTTON.size : Game::BUTTON.size;
    }

    [[nodiscard]] constexpr glm::vec2 get_position_end()
    {
        return get_position() + get_size();
    }

    void render(Game& t_game, bool t_target = false) const;
    [[nodiscard]] bool check_collision(GameState& t_game_state, const glm::vec2& t_mouse_position) const;

private:
    glm::vec2 m_position;
    const Sprite* m_text;
    glm::vec3 m_text_tint;
    bool m_big;
    std::optional<const Sprite*> m_icon;
};

class ButtonLayout final
{
public:
    constexpr ButtonLayout(std::vector<Button> t_buttons = {}, bool t_enabled = true) noexcept
        : m_buttons{std::move(t_buttons)}, m_enabled{t_enabled}
    {
    }

    constexpr void add_button(const Button& t_button)
    {
        m_buttons.emplace_back(t_button);
    }

    [[nodiscard]] constexpr const Button& operator[](std::size_t t_index) const noexcept
    {
        return m_buttons.at(t_index);
    }

    [[nodiscard]] constexpr const Button& last_button() const noexcept
    {
        return m_buttons.back();
    }

    void render(Game& t_game) const;

    [[nodiscard]] std::optional<std::size_t> get_clicked_button(GameState& t_game_state,
                                                                const glm::vec2& t_mouse_position) const;
    void update_target(GameState& t_game_state);

    constexpr void set_enabled_state(bool t_enabled) noexcept
    {
        m_enabled = t_enabled;
        if (!m_enabled)
        {
            m_target.reset();
        }
    }

private:
    std::vector<Button> m_buttons;
    std::optional<std::size_t> m_target;
    bool m_enabled;
};