#include "Transition.hpp"

#include "Game.hpp"

#include <SDL_opengles2.h>

Transition::Transition() noexcept
{
}

void Transition::update(Game& t_game)
{
    if (!is_transitioning())
    {
        return;
    }
    const auto time_passed = std::chrono::duration<float>(t_game.get_timepoint() - *m_tp_start).count();
    if (!m_half_done && time_passed >= 0.5F)
    {
        t_game.play_sfx<Game::SoundType::SWOOSH2>();
        m_half_done = true;
    }
    if (time_passed >= 1.0F)
    {
        stop_transition(t_game);
    }
}

void Transition::render(Game& t_game) const
{
    if (!is_transitioning())
    {
        return;
    }
    auto time_passed = std::chrono::duration<float>(t_game.get_timepoint() - *m_tp_start).count();
    const auto ease_in_out_quart = [](float t_x) {
        return t_x < 0.5F ? 8.0F * std::pow(t_x, 4.0F) : 1.0F - std::pow(-2.0F * t_x + 2.0F, 4.0F) * 0.5F;
    };
    if (is_half_done())
    {
        time_passed -= 0.5F;
        glViewport(0,
                   static_cast<GLint>(-float(t_game.get_window_size().y) +
                                      float(t_game.get_window_size().y) * ease_in_out_quart(time_passed * 2.0F)),
                   static_cast<GLsizei>(t_game.get_window_size().x), static_cast<GLsizei>(t_game.get_window_size().y));
    }
    else
    {
        glViewport(0, static_cast<GLint>(float(t_game.get_window_size().y) * ease_in_out_quart(time_passed * 2.0F)),
                   static_cast<GLsizei>(t_game.get_window_size().x), static_cast<GLsizei>(t_game.get_window_size().y));
    }
}

void Transition::trigger_transition(Game& t_game)
{
    if (is_transitioning())
    {
        return;
    }
    t_game.play_sfx<Game::SoundType::SWOOSH1>();
    m_half_done = false;
    m_tp_start = t_game.get_timepoint();
}

void Transition::stop_transition(Game& t_game)
{
    m_half_done = true;
    m_tp_start.reset();
    glViewport(0, 0, static_cast<GLsizei>(t_game.get_window_size().x),
               static_cast<GLsizei>(t_game.get_window_size().y));
}