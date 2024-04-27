#include "StateLevelSelect.hpp"

#include "Game.hpp"
#include "StateLevel.hpp"
#include "StateMenu.hpp"

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/transform.hpp>

#include <fstream>
#include <iostream>

StateLevelSelect::StateLevelSelect(class Game* t_game) noexcept : GameState{GameState::Type::LEVEL_SELECT, t_game}
{
    constexpr unsigned MAX_ROWS = 4;
    unsigned columns = 0;
    unsigned rows = 0;
    LevelInfo::Difficulty last_difficulty = LEVELS.begin()->difficulty;
    for (std::size_t index = 0; index < LEVELS.size(); ++index)
    {
        const auto& level = LEVELS[index];
        if (level.difficulty != last_difficulty || rows >= MAX_ROWS)
        {
            ++columns;
            rows = 0;
            last_difficulty = level.difficulty;
        }
        m_button_layout_levels.add_button(Button{{Game::MARGIN + columns * (Game::MARGIN + Game::BIG_BUTTON.size.x),
                                                  Game::MARGIN + rows * (Game::MARGIN + Game::BIG_BUTTON.size.y)},
                                                 [&]() -> const Sprite& {
                                                     switch (level.difficulty)
                                                     {
                                                     case LevelInfo::Difficulty::EASY:
                                                         return EASY_TEXT;
                                                     case LevelInfo::Difficulty::MEDIUM:
                                                         return MEDIUM_TEXT;
                                                     case LevelInfo::Difficulty::HARD:
                                                         return HARD_TEXT;
                                                     }
                                                 }(),
                                                 Game::COLOR_TEXT2,
                                                 true});
        ++rows;
    }
    ++columns;
    const float layout_y_bottom = Game::MARGIN + MAX_ROWS * (Game::MARGIN + Game::BIG_BUTTON.size.y);
    m_button_layout_bottom.add_button(Button{{Game::MARGIN, layout_y_bottom}, Game::BACK_TEXT});
    m_button_layout_bottom.add_button(
        Button{{Game::MARGIN + columns * (Game::MARGIN + Game::BIG_BUTTON.size.x) - Game::BUTTON.size.x - Game::MARGIN,
                layout_y_bottom},
               Game::RESET_TEXT,
               Game::COLOR_TEXT,
               false,
               &Game::TIMER_ICON});

    set_camera_center(glm::vec2{Game::MARGIN + columns * (Game::MARGIN + Game::BIG_BUTTON.size.x),
                                layout_y_bottom + Game::BUTTON.size.y + Game::MARGIN} *
                      0.5F);
    read_high_scores();
}

void StateLevelSelect::read_high_scores()
{
    const auto& paths = game().get_paths();
    for (std::size_t index = 0; index < LEVELS.size(); ++index)
    {
        std::intmax_t score{-1};
        std::ifstream file{paths.pref("level" + std::to_string(index) + ".dat")};
        file >> score;
        m_high_scores[index] = score;
    }
}

void StateLevelSelect::reset_high_scores()
{
    const auto& paths = game().get_paths();
    for (std::size_t index = 0; index < LEVELS.size(); ++index)
    {
        m_high_scores[index] = -1;
        std::ofstream file{paths.pref("level" + std::to_string(index) + ".dat"), std::ios::trunc};
        file << -1;
    }
}

void StateLevelSelect::handle_events()
{
    for (const auto& event : game().get_events())
    {
        if (game().is_transitioning())
        {
            break;
        }
        if (event.type == Event::Type::MOUSE_DOWN && event.button == Event::Button::LEFT)
        {
            const auto clicked_button_level = m_button_layout_levels.get_clicked_button(*this, event.coordinates);
            if (clicked_button_level)
            {
                Music::fade_out(0.5F);
                game().trigger_transition();
                m_selected_level = *clicked_button_level;
                break;
            }
            const auto clicked_button_bottom = m_button_layout_bottom.get_clicked_button(*this, event.coordinates);
            if (clicked_button_bottom)
            {
                const auto button = static_cast<Buttons>(*clicked_button_bottom);
                if (button == Buttons::BACK)
                {
                    game().trigger_transition();
                    m_back_clicked = true;
                    break;
                }
                if (button == Buttons::RESET_PB)
                {
                    game().play_sfx<Game::SoundType::BOMB>();
                    reset_high_scores();
                }
            }
        }
        if (event.type == Event::Type::KEYBOARD_DOWN && event.key == Event::Key::ESC)
        {
            m_back_clicked = true;
            game().trigger_transition();
        }
    }
}
void StateLevelSelect::draw_buttons()
{
    m_button_layout_bottom.update_target(*this);
    m_button_layout_levels.update_target(*this);
    m_button_layout_bottom.render(game());
    m_button_layout_levels.render(game());
    for (std::size_t index = 0; index < LEVELS.size(); ++index)
    {
        const auto& button_position = m_button_layout_levels[index].get_position();
        const auto& level_info = LEVELS[index];
        const auto dimension_info = std::to_string(level_info.size.x) + "x" + std::to_string(level_info.size.y);
        game().write_number(button_position + glm::vec2(LAYOUT_DIMENSION) +
                                glm::vec2(dimension_info.size() * (Game::NUMBERS.size.x + 1) - 1, 0),
                            dimension_info, Game::COLOR_TEXT2);
        game().write_number(button_position + glm::vec2(LAYOUT_BOMBS), std::to_string(level_info.bombs),
                            Game::COLOR_TEXT);
        const auto high_score = m_high_scores[index];
        if (high_score >= 0)
        {
            game().write_number(button_position + glm::vec2(LAYOUT_PB),
                                Game::format_duration(std::chrono::steady_clock::duration(high_score)),
                                Game::COLOR_TEXT);
        }
    }
}

void StateLevelSelect::update()
{
    if (game().is_half_done() && (m_selected_level || m_back_clicked))
    {
        if (m_back_clicked)
        {
            return game().change_state<StateMenu>();
        }
        const auto& level = LEVELS[*m_selected_level];
        if (level.difficulty == LevelInfo::Difficulty::EASY)
        {
            game().fade_in_music<Game::MusicType::EASY>(0.5F);
        }
        else if (level.difficulty == LevelInfo::Difficulty::MEDIUM)
        {
            game().fade_in_music<Game::MusicType::MEDIUM>(0.5F);
        }
        else if (level.difficulty == LevelInfo::Difficulty::HARD)
        {
            game().fade_in_music<Game::MusicType::HARD>(0.5F);
        }
        return game().change_state<StateLevel>(level.size, level.bombs, *m_selected_level);
    }
    handle_events();
    draw_buttons();
}