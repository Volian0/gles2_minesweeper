#include "Game.hpp"

#include "StateCredits.hpp"
#include "StateLevel.hpp"
#include "StateLevelSelect.hpp"
#include "StateMenu.hpp"

#include <glm/glm.hpp>

Game::Game()
    : m_window{"Minesweeper", INITIAL_WINDOW_SIZE - glm::uvec2{1}}, m_gl_context{m_window},
      m_paths{"Volian", "Minesweeper"}, m_quad_renderer{TINT3D_ATTRIBUTES},
      m_sounds{Sound{m_paths.base("res/sound/hover.wav")},  Sound{m_paths.base("res/sound/flag.wav")},
               Sound{m_paths.base("res/sound/unflag.wav")}, Sound{m_paths.base("res/sound/uncover.wav")},
               Sound{m_paths.base("res/sound/bomb.wav")},   Sound{m_paths.base("res/sound/swoosh1.wav")},
               Sound{m_paths.base("res/sound/swoosh2.wav")}},
      m_musics{Music{m_paths.base("res/music/menu.wav")}, Music{m_paths.base("res/music/easy.wav")},
               Music{m_paths.base("res/music/medium.wav")}, Music{m_paths.base("res/music/hard.wav")}},
      m_shader{TINT3D_ATTRIBUTES, m_paths.base("res/shader/tint3d.vert"), m_paths.base("res/shader/tint3d.frag")},
      m_uniform_matrix{m_shader.get_uniform_location("u_matrix")}
{
    glEnable(GL_CULL_FACE);
    glClearColor(COLOR_CLEAR.r, COLOR_CLEAR.g, COLOR_CLEAR.b, 1);
    glActiveTexture(GL_TEXTURE0);
    m_texture_cell.bind();
    Texture::image(m_paths.base("res/texture/atlas.png"));
    m_shader.use();
    glUniform1i(m_shader.get_uniform_location("u_sampler"), 0);
    SDL_SetWindowSize(m_window.ptr(), INITIAL_WINDOW_SIZE.x, INITIAL_WINDOW_SIZE.y);
    change_state<StateMenu>(true);
}

void Game::stop()
{
    m_state.reset();
    m_state_changed = true;
}

void Game::trigger_transition()
{
    m_state_transition.trigger_transition(*this);
}

void Game::update_mouse_position()
{
    glm::ivec2 mouse_position;
    SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
    m_mouse_position = static_cast<glm::uvec2>(mouse_position);
}

void Game::update_matrix(const glm::mat4& t_matrix)
{
    glUniformMatrix4fv(m_uniform_matrix, 1, GL_FALSE, glm::value_ptr(t_matrix));
}

void Game::handle_events()
{
    m_events.clear();
    SDL_Event event;
    while (SDL_PollEvent(&event) == SDL_TRUE)
    {
        if (event.type == SDL_QUIT)
        {
            return stop();
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        {
            static constexpr auto SQUARE_ANGLE = glm::radians(90.0F);
            static constexpr auto Z_NEAR = 0.01F;
            static constexpr auto Z_FAR = 100000.0F;
            m_window_size = {event.window.data1, event.window.data2};
            m_aspect_ratio = float(m_window_size.x) / float(m_window_size.y);
            glViewport(0, 0, static_cast<GLsizei>(m_window_size.x), static_cast<GLsizei>(m_window_size.y));
            m_matrix_projection = glm::perspective(SQUARE_ANGLE, m_aspect_ratio, Z_NEAR, Z_FAR);
            m_events.emplace_back(Event{Event::Type::WINDOW_RESIZE, {}, {}, m_window_size});
            m_state->update_matrix();
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            glm::uvec2 coordinates = {event.button.x, event.button.y};
            Event::Button button = Event::Button::NONE;
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                button = Event::Button::LEFT;
            }
            else if (event.button.button == SDL_BUTTON_RIGHT)
            {
                button = Event::Button::RIGHT;
            }
            if (button != Event::Button::NONE)
            {
                m_events.emplace_back(Event{Event::Type::MOUSE_DOWN, button, {}, coordinates});
            }
        }
        else if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
        {
            Event::Key key = Event::Key::NONE;
            if (event.key.keysym.sym == SDLK_r)
            {
                key = Event::Key::R;
            }
            else if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                key = Event::Key::ESC;
            }
            if (key != Event::Key::NONE)
            {
                m_events.emplace_back(Event{Event::Type::KEYBOARD_DOWN, {}, key, {}});
            }
        }
    }
    update_mouse_position();
}

[[nodiscard]] double Game::time_modulo(double t_seconds) const noexcept
{
    return std::fmod(m_elapsed_time, t_seconds);
}

void Game::run()
{
    while (m_state)
    {
        m_tp_new = std::chrono::steady_clock::now();
        m_delta_time = std::chrono::duration<float>(m_tp_new - m_tp_old).count();
        m_elapsed_time += m_delta_time;
        m_target_tint = time_modulo(0.25) > 0.125 ? COLOR_BLACK : COLOR_GRAY;
        m_tp_old = m_tp_new;
        handle_events();
        if (!m_state)
        {
            return;
        }
        m_quad_renderer.clear();
        m_state_transition.update(*this);
        switch (m_state->get_type())
        {
        case GameState::Type::MAIN_MENU:
            static_cast<StateMenu&>(*m_state).update();
            break;
        case GameState::Type::LEVEL_SELECT:
            static_cast<StateLevelSelect&>(*m_state).update();
            break;
        case GameState::Type::LEVEL:
            static_cast<StateLevel&>(*m_state).update();
            break;
        case GameState::Type::CREDITS:
            static_cast<StateCredits&>(*m_state).update();
            break;
        }
        if (m_state_changed)
        {
            m_state_changed = false;
            continue;
        }
        m_quad_renderer.draw();
        m_state_transition.render(*this);
        SDL_GL_SwapWindow(m_window.ptr());
    }
}

std::string Game::format_duration(const std::chrono::steady_clock::duration& t_duration)
{
    auto duration = std::chrono::duration_cast<std::chrono::duration<std::intmax_t, std::centi>>(t_duration).count();
    constexpr auto CENTISECONDS_IN_MINUTE = 6000;
    const auto duration_minutes = duration / CENTISECONDS_IN_MINUTE;
    duration %= CENTISECONDS_IN_MINUTE;
    std::string duration_string{std::to_string(duration)};
    if (duration_string.size() < 4)
    {
        duration_string.insert(0, 4 - duration_string.size(), '0');
    }
    duration_string.insert(duration_string.size() - 2, 1, '.');
    if (duration_minutes > 0)
    {
        duration_string.insert(0, std::to_string(duration_minutes) + ":");
    }
    return duration_string;
}

void Game::write_number(glm::vec2 t_position, std::string_view t_number, const glm::vec3& t_tint)
{
    for (const auto number_char : t_number | std::views::reverse)
    {
        if (number_char == '.')
        {
            t_position.x -= DOT_CHAR.size.x;
            draw(DOT_CHAR, t_position, t_tint);
        }
        else if (number_char == ':')
        {
            t_position.x -= COLON_CHAR.size.x;
            draw(COLON_CHAR, t_position, t_tint);
        }
        else if (number_char == '/')
        {
            t_position.x -= SLASH_CHAR.size.x;
            draw(SLASH_CHAR, t_position, t_tint);
        }
        else if (number_char == 'x')
        {
            t_position.x -= X_CHAR.size.x;
            draw(X_CHAR, t_position, t_tint);
        }
        else
        {
            t_position.x -= NUMBERS.size.x;
            draw(NUMBERS, t_position, t_tint, {NUMBERS.size.x * (number_char - '0'), 0});
        }
        t_position.x -= 1.0F;
    }
}

void Game::draw(const Sprite& t_sprite, const glm::vec2& t_position, const glm::vec3& t_tint, const glm::vec2& t_offset,
                const glm::vec2& t_additional_size)
{
    const std::array<glm::vec2, 2> positions{t_position, t_position + glm::vec2(t_sprite.size * t_sprite.scale) +
                                                             t_additional_size};
    const auto starting_uv = (glm::vec2(t_sprite.position) + t_offset) * ATLAS_SCALE;
    const std::array<glm::vec2, 2> uv_positions{
        starting_uv, starting_uv + (glm::vec2(t_sprite.size) + t_additional_size) * ATLAS_SCALE};
    m_quad_renderer.add_quad({positions[0], glm::vec2{positions[1].x, positions[0].y}, positions[1],
                              glm::vec2{positions[0].x, positions[1].y}},
                             {uv_positions[0], glm::vec2{uv_positions[1].x, uv_positions[0].y}, uv_positions[1],
                              glm::vec2{uv_positions[0].x, uv_positions[1].y}},
                             t_tint);
}

void Game::draw_advanced(const Sprite& t_sprite, const glm::vec2& t_position, float t_angle, const glm::vec2& t_origin,
                         const glm::vec3& t_tint, float t_scale, const glm::vec2& t_additional_size)
{
    const std::array<glm::vec2, 2> positions{t_position, t_position + glm::vec2(t_sprite.size * t_sprite.scale) +
                                                             t_additional_size};
    const std::array<glm::vec2, 2> uv_positions{glm::vec2(t_sprite.position) * ATLAS_SCALE,
                                                (glm::vec2(t_sprite.position + t_sprite.size) + t_additional_size) *
                                                    ATLAS_SCALE};
    std::array<glm::vec2, 4> vertices{{positions[0], glm::vec2{positions[1].x, positions[0].y}, positions[1],
                                       glm::vec2{positions[0].x, positions[1].y}}};
    const auto sine = std::sin(t_angle);
    const auto cosine = std::cos(t_angle);
    for (auto& vertex : vertices)
    {
        vertex -= t_origin;
        vertex *= t_scale;
        vertex = glm::vec2{vertex.x * cosine - vertex.y * sine, vertex.x * sine + vertex.y * cosine};
        vertex += t_origin;
    }
    m_quad_renderer.add_quad(vertices,
                             {uv_positions[0], glm::vec2{uv_positions[1].x, uv_positions[0].y}, uv_positions[1],
                              glm::vec2{uv_positions[0].x, uv_positions[1].y}},
                             t_tint);
}