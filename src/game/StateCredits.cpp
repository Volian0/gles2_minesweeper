#include "StateCredits.hpp"

#include "Game.hpp"
#include "StateMenu.hpp"

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <utility>

StateCredits::StateCredits(class Game* t_game)
    : GameState{GameState::Type::CREDITS, t_game}, m_layout_y_logo{Game::MARGIN},
      m_layout_y_credits{m_layout_y_logo + Game::LOGO.size.y * Game::LOGO.scale + Game::SMALL_MARGIN},
      m_layout_y_game{m_layout_y_credits + Game::CREDITS_TEXT.size.y + Game::MARGIN},
      m_layout_y_music{m_layout_y_game + Game::GAME_TEXT.size.y + Game::SMALL_MARGIN},
      m_layout_y_sfx{m_layout_y_music + MUSIC_CREDITS.size.y + Game::SMALL_MARGIN},
      m_layout_x_size{Game::MARGIN * 2 + MUSIC_CREDITS.size.x},
      m_button_layout{
          {Button{{(m_layout_x_size - Game::BUTTON.size.x) * 0.5F, m_layout_y_sfx + SFX_CREDITS.size.y + Game::MARGIN},
                  Game::BACK_TEXT}}},
      m_layout_game_volian0_size{Game::GAME_TEXT.size.x + Game::GAME_TEXT.offset + 2 + Game::VOLIAN_TEXT.size.x}
{
    set_camera_center({m_layout_x_size * 0.5F,
                       (m_button_layout.last_button().get_position().y + Game::BUTTON.size.y + Game::MARGIN) * 0.5F});
}

void StateCredits::update()
{
    if (m_back_clicked && game().is_half_done())
    {
        return game().change_state<StateMenu>();
    }
    for (const auto& event : game().get_events())
    {
        if (game().is_transitioning())
        {
            break;
        }
        if ((event.type == Event::Type::MOUSE_DOWN && event.button == Event::Button::LEFT &&
             m_button_layout.get_clicked_button(*this, event.coordinates)) ||
            (event.type == Event::Type::KEYBOARD_DOWN && event.key == Event::Key::ESC))
        {
            m_back_clicked = true;
            game().trigger_transition();
        }
    }

    constexpr std::array<std::pair<float, float>, 3> ANGLE_PARAMETERS{
        std::pair<float, float>{1.4312F, 1.54353F}, {1.554353F, 0.7535454F}, {1.63543F, 2.145345F}};

    std::array<std::pair<float, float>, 3> angles_and_scales{};
    for (std::size_t index = 0; index < angles_and_scales.size(); ++index)
    {
        const auto cosine_argument =
            float(game().get_elapsed_time()) * ANGLE_PARAMETERS[index].first + ANGLE_PARAMETERS[index].second;
        float lerp = (float(std::cos(cosine_argument)) + 1.0F) / 2.0F;
        const float sign = std::cos(cosine_argument * 0.5F) < 0.0F ? -1.0F : 1.0F;
        const auto angle = lerp * sign * 0.025F;
        const auto scale = 1.0F - std::abs(angle) * 2.0F;
        angles_and_scales[index].first = angle;
        angles_and_scales[index].second = scale;
    }
    game().draw(Game::LOGO, {(m_layout_x_size - Game::LOGO.size.x * Game::LOGO.scale) * 0.5F, m_layout_y_logo});
    game().draw(
        Game::CREDITS_TEXT,
        {(m_layout_x_size - (Game::CREDITS_TEXT.size.x + Game::CREDITS_TEXT.offset)) * 0.5F, m_layout_y_credits},
        Game::COLOR_GRAY, {}, {Game::CREDITS_TEXT.offset, 0.0F});
    game().draw_advanced(Game::GAME_TEXT, {(m_layout_x_size - (m_layout_game_volian0_size)) * 0.5F, m_layout_y_game},
                         angles_and_scales[0].first,
                         glm::vec2(m_layout_x_size * 0.5F, m_layout_y_game + Game::GAME_TEXT.size.y * 0.5F),
                         glm::vec3{1.0F}, angles_and_scales[0].second, {Game::GAME_TEXT.offset, 0.0F});
    game().draw_advanced(Game::VOLIAN_TEXT,
                         {(m_layout_x_size - (m_layout_game_volian0_size)) * 0.5F + Game::GAME_TEXT.size.x +
                              Game::GAME_TEXT.offset + 2.0F,
                          m_layout_y_game},
                         angles_and_scales[0].first,
                         glm::vec2(m_layout_x_size * 0.5F, m_layout_y_game + Game::VOLIAN_TEXT.size.y * 0.5F),
                         glm::vec3{1.0F}, angles_and_scales[0].second);
    game().draw_advanced(MUSIC_CREDITS, {(m_layout_x_size - MUSIC_CREDITS.size.x) * 0.5F, m_layout_y_music},
                         angles_and_scales[1].first,
                         {m_layout_x_size * 0.5F, m_layout_y_music + MUSIC_CREDITS.size.y * 0.5F}, glm::vec3{1.0F},
                         angles_and_scales[1].second);
    game().draw_advanced(SFX_CREDITS, {(m_layout_x_size - SFX_CREDITS.size.x) * 0.5F, m_layout_y_sfx},
                         angles_and_scales[2].first,
                         {m_layout_x_size * 0.5F, m_layout_y_sfx + SFX_CREDITS.size.y * 0.5F}, glm::vec3{1.0F},
                         angles_and_scales[2].second);
    m_button_layout.update_target(*this);
    m_button_layout.render(game());
}
