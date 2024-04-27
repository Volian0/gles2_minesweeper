#pragma once

#include <chrono>
#include <optional>

class Transition final
{
public:
    Transition() noexcept;

    void update(class Game& t_game);
    void render(class Game& t_game) const;

    void trigger_transition(class Game& t_game);

    [[nodiscard]] constexpr bool is_transitioning() const noexcept
    {
        return m_tp_start.has_value();
    }

    [[nodiscard]] constexpr bool is_half_done() const noexcept
    {
        return m_half_done;
    }

private:
    void stop_transition(class Game& t_game);
    std::optional<std::chrono::steady_clock::time_point> m_tp_start;
    bool m_half_done{true};
};