#include "StateMenu.hpp"

#include "Game.hpp"
#include "StateCredits.hpp"
#include "StateLevelSelect.hpp"

StateMenu::StateMenu(class Game* t_game, bool t_play_music)
    : GameState{Type::MAIN_MENU, t_game}, m_layout_y_logo{Game::MARGIN},
      m_layout_x_size{Game::MARGIN * 2 + Game::LOGO.size.x * Game::LOGO.scale},
      m_layout_first_button{(m_layout_x_size - Game::BUTTON.size.x) * 0.5F,
                            Game::LOGO.size.y * Game::LOGO.scale + m_layout_y_logo + Game::MARGIN},
      m_button_layout{
          {Button{{m_layout_first_button.x, m_layout_first_button.y}, PLAY_TEXT},
           Button{{m_layout_first_button.x, m_layout_first_button.y + 1 * (Game::BUTTON.size.y + Game::SMALL_MARGIN)},
                  Game::CREDITS_TEXT},
           Button{{m_layout_first_button.x, m_layout_first_button.y + 2 * (Game::BUTTON.size.y + Game::SMALL_MARGIN)},
                  Game::EXIT_TEXT}}},
      m_layout_game_text{
          (m_layout_x_size - (Game::GAME_TEXT.size.x + 2 + BY_TEXT.size.x + 2 + Game::VOLIAN_TEXT.size.x)) * 0.5F,
          m_button_layout.last_button().get_position().y + Game::MARGIN + Game::BUTTON.size.y}
{
    set_camera_center(glm::vec2{m_layout_x_size, m_layout_game_text.y + Game::MARGIN + Game::GAME_TEXT.size.y} * 0.5F);
    if (t_play_music)
    {
        Mix_VolumeMusic(64);
        game().fade_in_music<Game::MusicType::MENU>(0.5F);
    }
}

void StateMenu::update()
{
    if (m_clicked_button && game().is_half_done())
    {
        if (m_clicked_button == Buttons::PLAY)
        {
            return game().change_state<StateLevelSelect>();
        }
        if (m_clicked_button == Buttons::CREDITS)
        {
            return game().change_state<StateCredits>();
        }
    }
    for (const auto& event : game().get_events())
    {
        if (game().is_transitioning())
        {
            break;
        }
        if (event.type == Event::Type::KEYBOARD_DOWN && event.key == Event::Key::ESC)
        {
            return game().stop();
        }
        if (event.type == Event::Type::MOUSE_DOWN && event.button == Event::Button::LEFT)
        {
            const auto clicked_button = m_button_layout.get_clicked_button(*this, event.coordinates);
            if (clicked_button)
            {
                if (static_cast<Buttons>(*clicked_button) == Buttons::EXIT)
                {
                    return game().stop();
                }
                game().trigger_transition();
                m_clicked_button = static_cast<Buttons>(*clicked_button);
            }
        }
    }
    float logo_lerp = (float(std::cos(game().get_elapsed_time() * 1.5F)) + 1.0F) / 2.0F;
    const float sign = std::cos(game().get_elapsed_time() * 1.5F * 0.5F) < 0.0F ? -1.0F : 1.0F;
    const float logo_angle = logo_lerp * sign * 0.03F;
    const auto logo_scale = 1.0F - std::abs(logo_angle) * 2.0F;
    game().draw_advanced(Game::LOGO, {Game::MARGIN, m_layout_y_logo}, logo_angle,
                         glm::vec2{Game::MARGIN, m_layout_y_logo} + glm::vec2{Game::LOGO.size}, glm::vec3{1.0F},
                         logo_scale);
    game().draw(Game::GAME_TEXT, m_layout_game_text);
    const float by_text_x = Game::GAME_TEXT.size.x + 2.0F;
    game().draw(BY_TEXT, m_layout_game_text + glm::vec2(by_text_x, 0.0F));
    game().draw(Game::VOLIAN_TEXT, m_layout_game_text + glm::vec2(by_text_x + BY_TEXT.size.x + 2.0F, 0.0F));
    m_button_layout.update_target(*this);
    m_button_layout.render(game());
}