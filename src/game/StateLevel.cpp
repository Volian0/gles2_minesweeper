#include "StateLevel.hpp"

#include "Game.hpp"
#include "StateLevelSelect.hpp"

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/transform.hpp>

#include <cstdint>
#include <fstream>

StateLevel::StateLevel(class Game* t_game, glm::uvec2 t_minesweeper_size, unsigned t_bombs_number,
                       unsigned t_level_number) noexcept
    : GameState{GameState::Type::LEVEL,
                t_game,
                {Game::MARGIN + float(t_minesweeper_size.x) * COVERED_CELL.size.x * 0.5F,
                 Game::MARGIN * 2.0F + Game::NUMBERS.size.y +
                     float(t_minesweeper_size.y) * COVERED_CELL.size.y * 0.5F}},
      m_minesweeper{t_minesweeper_size, t_bombs_number}, m_layout_y_top{Game::MARGIN},
      m_layout_y_bottom{Game::MARGIN * 2.0F + FLAG_ICON.size.y + float(m_minesweeper.size().y) * COVERED_CELL.size.y +
                        Game::MARGIN},
      m_layout_x_icon{float(Game::MARGIN) + float(m_minesweeper.size().x) * COVERED_CELL.size.x - FLAG_ICON.size.x},
      m_layout_x_numbers{m_layout_x_icon - 1.0F},
      m_layout_cells{Game::MARGIN, Game::MARGIN * 2.0F + Game::NUMBERS.size.y},
      m_layout_button_offset{static_cast<float>((float(t_minesweeper_size.x) * COVERED_CELL.size.x) / 2.0F -
                                                float(Game::BUTTON.size.x) - 2.0F)},
      m_button_layout{
          {Button{glm::vec2(Game::MARGIN + m_layout_button_offset, m_layout_y_bottom - Game::BUTTON.offset),
                  Game::EXIT_TEXT},
           Button{glm::vec2(float(Game::MARGIN + m_minesweeper.size().x * COVERED_CELL.size.x - Game::BUTTON.size.x) -
                                m_layout_button_offset,
                            m_layout_y_bottom - Game::BUTTON.offset),
                  Game::RESET_TEXT}}},
      m_level_number{t_level_number}
{
}

void StateLevel::left_click(const glm::uvec2& t_mouse_cell)
{
    const auto result = m_minesweeper.uncover(t_mouse_cell, game().get_timepoint(), game().get_rng_generator());
    float acceleration_multiplier{10.0F};
    float velocity_y{0.0F};
    if (result.action == Minesweeper::UncoverAction::UNCOVER)
    {
        game().play_sfx<Game::SoundType::UNCOVER>();
        if (m_minesweeper.get_game_state() == Minesweeper::GameState::WON)
        {
            const auto& paths = game().get_paths();
            const auto file_path = paths.pref("level" + std::to_string(m_level_number) + ".dat");
            std::intmax_t best_score{-1};
            const std::intmax_t new_score = (*m_minesweeper.get_duration(game().get_timepoint())).count();
            {
                std::ifstream file{file_path};
                file >> best_score;
            }
            if (best_score < 0 || new_score < best_score)
            {
                std::ofstream file{file_path, std::ios::trunc};
                file << new_score;
            }
        }
    }
    else if (result.action == Minesweeper::UncoverAction::EXPLOSION)
    {
        m_tp_explosion = game().get_timepoint();
        game().play_sfx<Game::SoundType::BOMB>();
        acceleration_multiplier *= 2.0F;
        velocity_y = m_rng_distribution(game().get_rng_generator()) * 20.0F;
        Mix_VolumeMusic(24);
    }
    for (const auto& cell : result.uncovered_cells)
    {
        m_particles.emplace_back(ParticleCell{
            cell, glm::vec2(0.0F, velocity_y),
            glm::vec2(m_rng_distribution(game().get_rng_generator()) * acceleration_multiplier, GRAVITY_ACCELERATION),
            0.0F});
    }
}

void StateLevel::right_click(const glm::uvec2& t_mouse_cell)
{
    const auto action = m_minesweeper.flag(t_mouse_cell);
    if (action == Minesweeper::FlagAction::FLAG)
    {
        game().play_sfx<Game::SoundType::FLAG>();
    }
    else if (action == Minesweeper::FlagAction::UNFLAG)
    {
        game().play_sfx<Game::SoundType::UNFLAG>();
    }
}

void StateLevel::reset()
{
    m_clicked_button = Buttons::RESET;
    game().trigger_transition();
}

void StateLevel::exit()
{
    m_clicked_button = Buttons::EXIT;
    game().trigger_transition();
    Music::fade_out(0.5F);
}

void StateLevel::handle_button_click(const glm::uvec2& t_mouse_position)
{
    const auto clicked_button = m_button_layout.get_clicked_button(*this, t_mouse_position);
    if (clicked_button)
    {
        const auto button = static_cast<Buttons>(*clicked_button);
        if (button == Buttons::EXIT)
        {
            exit();
        }
        else if (button == Buttons::RESET)
        {
            reset();
        }
    }
}

void StateLevel::handle_events()
{
    const auto click = [&](const Event& t_event) {
        if (t_event.button == Event::Button::LEFT && (m_minesweeper.get_game_state() == Minesweeper::GameState::LOST ||
                                                      m_minesweeper.get_game_state() == Minesweeper::GameState::WON))
        {
            handle_button_click(t_event.coordinates);
        }
        const auto mouse_cell = get_mouse_cell(t_event.coordinates);
        if (!mouse_cell)
        {
            return;
        }
        if (t_event.button == Event::Button::LEFT)
        {
            left_click(*mouse_cell);
        }
        else if (t_event.button == Event::Button::RIGHT)
        {
            right_click(*mouse_cell);
        }
    };
    for (const auto& event : game().get_events())
    {
        if (game().is_transitioning())
        {
            break;
        }
        if (event.type == Event::Type::MOUSE_DOWN)
        {
            click(event);
        }
        else if (event.type == Event::Type::KEYBOARD_DOWN)
        {
            if (event.key == Event::Key::ESC)
            {
                exit();
            }
            else if (event.key == Event::Key::R &&
                     m_minesweeper.get_game_state() != Minesweeper::GameState::NOT_STARTED)
            {
                reset();
            }
        }
    }
}
void StateLevel::draw_timer()
{
    if (m_minesweeper.get_game_state() != Minesweeper::GameState::LOST)
    {
        auto duration = m_minesweeper.get_duration(game().get_timepoint());
        auto timer_string = duration.has_value() ? Game::format_duration(*duration) : std::string{"00.00"};
        if (m_minesweeper.get_game_state() != Minesweeper::GameState::WON ||
            game().time_modulo(0.5) < (2.0 / 3.0) * 0.5)
        {
            game().write_number({m_layout_x_numbers, m_layout_y_top}, timer_string, Game::COLOR_TEXT);
        }
        game().draw(Game::TIMER_ICON, glm::vec2(m_layout_x_icon, m_layout_y_top));
    }
}
void StateLevel::draw_flags()
{
    if (m_minesweeper.get_game_state() == Minesweeper::GameState::PLAYING ||
        m_minesweeper.get_game_state() == Minesweeper::GameState::NOT_STARTED)
    {
        const auto tint_color = m_minesweeper.get_used_flags() > m_minesweeper.get_bomb_amount()
                                    ? game().get_target_tint()
                                    : Game::COLOR_TEXT;

        game().write_number({m_layout_x_numbers, m_layout_y_bottom},
                            std::to_string(m_minesweeper.get_used_flags()) + "/" +
                                std::to_string(m_minesweeper.get_bomb_amount()),
                            tint_color);

        game().draw(FLAG_ICON, glm::vec2(m_layout_x_icon, m_layout_y_bottom));
    }
}
void StateLevel::draw_cells()
{
    const auto draw_cell = [&](const glm::uvec2& t_cell) {
        auto sprite = COVERED_CELL;
        const auto cell_state = m_minesweeper.get_game_state() == Minesweeper::GameState::NOT_STARTED
                                    ? Minesweeper::CellState::COVERED
                                    : m_minesweeper.get_cell_state(t_cell);
        if (cell_state == Minesweeper::CellState::FLAGGED)
        {
            sprite =
                m_minesweeper.get_game_state() == Minesweeper::GameState::LOST ? WRONGLY_FLAGGED_CELL : FLAGGED_CELL;
        }
        else if (cell_state == Minesweeper::CellState::UNCOVERED)
        {
            sprite = UNCOVERED_CELL;
            if (m_minesweeper.is_bomb(t_cell))
            {
                sprite = t_cell == m_minesweeper.get_exploded_cell() ? TRIGGERED_MINE_CELL : MINE_CELL;
            }
        }
        game().draw(sprite, glm::vec2(t_cell * sprite.size) + m_layout_cells);
        if (cell_state == Minesweeper::CellState::UNCOVERED && !m_minesweeper.is_bomb(t_cell))
        {
            const auto nearby = m_minesweeper.get_nearby_mines(t_cell);
            if (nearby != 0)
            {
                constexpr std::array<glm::vec3, 3> COLORS{Game::COLOR_TEXT, Game::COLOR_GRAY, Game::COLOR_BLACK};
                constexpr std::array<std::size_t, 8> NUMBER_COLORS{2, 2, 2, 3, 3, 3, 1, 1};
                game().draw(Game::NUMBERS,
                            glm::vec2(t_cell * sprite.size) + m_layout_cells +
                                glm::vec2(UNCOVERED_CELL.size - Game::NUMBERS.size) * 0.5F,
                            COLORS[NUMBER_COLORS[nearby - 1] - 1], {Game::NUMBERS.size.x * nearby, 0});
            }
        }
    };
    for (unsigned y_grid = 0; y_grid < m_minesweeper.size().y; ++y_grid)
    {
        for (unsigned x_grid = 0; x_grid < m_minesweeper.size().x; ++x_grid)
        {
            draw_cell({x_grid, y_grid});
        }
    }
}
void StateLevel::draw_particles()
{
    for (auto& particle : m_particles)
    {
        particle.update(game().get_delta_time());
    }
    m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(),
                                     [](const auto& particle) { return particle.accumulated_time > 4.0F; }),
                      m_particles.end());
    for (const auto& particle : m_particles)
    {
        float angle = glm::atan(-particle.velocity.x, particle.velocity.y);
        const auto particle_position = particle.position * glm::vec2(COVERED_CELL.size) + m_layout_cells;
        game().draw_advanced(COVERED_CELL, particle_position, angle,
                             particle_position + glm::vec2(COVERED_CELL.size) * 0.5F);
    }
}
void StateLevel::draw_buttons()
{
    m_button_layout.set_enabled_state(m_minesweeper.get_game_state() == Minesweeper::GameState::LOST ||
                                      m_minesweeper.get_game_state() == Minesweeper::GameState::WON);
    m_button_layout.update_target(*this);
    m_button_layout.render(game());
}

void StateLevel::handle_shake()
{
    if (m_tp_explosion)
    {
        const auto time = std::chrono::duration<float>{game().get_timepoint() - *m_tp_explosion}.count();
        if (time >= 0.5F)
        {
            m_tp_explosion.reset();
            update_matrix();
        }
        else
        {
            const auto intensity = (0.5F - time) * 2.0F;
            update_matrix(glm::vec2{float(std::sin(time * 179.4)) * 3.0F, std::sin(time * 110.0) * 2.0F} * intensity,
                          float(std::cos(time * 130.0)) * 0.12F * intensity);
        }
    }
}

void StateLevel::draw_cell_target()
{
    const auto mouse_cell = get_mouse_cell(game().get_mouse_position());
    if (mouse_cell && m_minesweeper.can_target(*mouse_cell))
    {
        game().draw(CELL_TARGET, glm::vec2(*mouse_cell) * glm::vec2(COVERED_CELL.size) + m_layout_cells,
                    game().get_target_tint());
        if (mouse_cell != m_target_cell)
        {
            m_target_cell = mouse_cell;
            game().play_sfx<Game::SoundType::HOVER>();
        }
    }
    else
    {
        m_target_cell = std::nullopt;
    }
}

void StateLevel::update()
{
    if (m_clicked_button && game().is_half_done())
    {
        if (m_clicked_button == Buttons::RESET)
        {
            m_minesweeper.restart();
            m_particles.clear();
            Mix_VolumeMusic(64);
            m_tp_explosion.reset();
            update_matrix();
            m_clicked_button.reset();
        }
        else if (m_clicked_button == Buttons::EXIT)
        {
            Mix_VolumeMusic(64);
            game().fade_in_music<Game::MusicType::MENU>(0.5F);
            return game().change_state<StateLevelSelect>();
        }
    }
    handle_events();
    draw_cells();
    handle_shake();
    draw_particles();
    draw_cell_target();
    draw_timer();
    draw_flags();
    draw_buttons();
}

[[nodiscard]] std::optional<glm::uvec2> StateLevel::get_mouse_cell(const glm::uvec2& t_mouse_position) const noexcept
{
    if (game().is_transitioning())
    {
        return std::nullopt;
    }
    std::optional<glm::uvec2> mouse_cell;
    auto mouse_f = glm::vec2{t_mouse_position} / glm::vec2{game().get_window_size()};
    const auto cell_size_f =
        glm::vec2{glm::vec2(COVERED_CELL.size) / -get_camera_z() * 0.5F} / glm::vec2{game().get_aspect_ratio(), 1.0F};
    const auto cell_starting_x = (0.5F - float(m_minesweeper.size().x * COVERED_CELL.size.x) * 0.5F / -get_camera_z() *
                                             0.5F / game().get_aspect_ratio());
    const auto cell_starting_y =
        (0.5F - float(m_minesweeper.size().y * COVERED_CELL.size.y) * 0.5F / -get_camera_z() * 0.5F);
    glm::ivec2 tmp = glm::floor((mouse_f - glm::vec2(cell_starting_x, cell_starting_y)) / cell_size_f);
    if (tmp.x >= 0 && tmp.y >= 0 && tmp.x < m_minesweeper.size().x && tmp.y < m_minesweeper.size().y)
    {
        mouse_cell = tmp;
    }
    return mouse_cell;
}