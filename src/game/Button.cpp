#include "Button.hpp"

void Button::render(Game& t_game, bool t_target) const
{
    t_game.draw(m_big ? Game::BIG_BUTTON : Game::BUTTON, m_position);
    if (m_big)
    {
        t_game.draw(*m_text, m_position + glm::vec2{Game::BIG_BUTTON.offset}, m_text_tint);
    }
    else
    {
        if (m_icon)
        {
            t_game.draw(*m_text, m_position + glm::vec2{Game::BUTTON.offset}, m_text_tint);
            t_game.draw(**m_icon,
                        m_position + glm::vec2{Game::BUTTON.offset} + glm::vec2{1.0F + float(m_text->size.x), 0.0F});
        }
        else
        {
            t_game.draw(*m_text,
                        m_position + glm::vec2{0.5F * float(Game::BUTTON.size.x - m_text->size.x), Game::BUTTON.offset},
                        m_text_tint);
        }
    }
    if (t_target)
    {
        t_game.draw(m_big ? Game::BIG_BUTTON_TARGET : Game::BUTTON_TARGET, m_position, t_game.get_target_tint());
    }
}

[[nodiscard]] bool Button::check_collision(GameState& t_game_state, const glm::vec2& t_mouse_position) const
{
    return t_game_state.does_button_collide(m_position, get_size(), t_mouse_position);
}

[[nodiscard]] std::optional<std::size_t> ButtonLayout::get_clicked_button(GameState& t_game_state,
                                                                          const glm::vec2& t_mouse_position) const
{
    for (std::size_t index = 0; index < m_buttons.size(); ++index)
    {
        if (m_buttons[index].check_collision(t_game_state, t_mouse_position))
        {
            return index;
        }
    }
    return std::nullopt;
}

void ButtonLayout::render(Game& t_game) const
{
    if (!m_enabled)
    {
        return;
    }
    for (std::size_t index = 0; index < m_buttons.size(); ++index)
    {
        m_buttons[index].render(t_game, index == m_target);
    }
}

void ButtonLayout::update_target(GameState& t_game_state)
{
    if (!m_enabled)
    {
        return;
    }
    if (!t_game_state.game().is_transitioning())
    {
        const auto& mouse_position = t_game_state.game().get_mouse_position();
        for (std::size_t index = 0; index < m_buttons.size(); ++index)
        {
            if (m_buttons[index].check_collision(t_game_state, mouse_position))
            {
                if (m_target != index)
                {
                    t_game_state.game().play_sfx<Game::SoundType::HOVER>();
                }
                m_target = index;
                return;
            }
        }
    }
    m_target = std::nullopt;
}