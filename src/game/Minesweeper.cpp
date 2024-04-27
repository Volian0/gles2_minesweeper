#include "Minesweeper.hpp"

[[nodiscard]] Minesweeper::UncoverResult Minesweeper::uncover(
    const glm::uvec2& t_cell, const std::chrono::time_point<std::chrono::steady_clock>& t_tp,
    std::mt19937& t_rng_generator) noexcept
{
    UncoverResult uncover_result{UncoverAction::NOTHING, {}};
    if (!is_valid_position(t_cell) || m_game_state == GameState::WON || m_game_state == GameState::LOST)
    {
        return uncover_result;
    }
    const auto check_win_condition = [&]() { return m_uncovered_cells == m_size.x * m_size.y - m_bomb_amount; };
    if (m_game_state == GameState::NOT_STARTED)
    {
        uncover_result.action = UncoverAction::UNCOVER;
        m_tp_start = t_tp;
        while (true)
        {
            generate(t_cell, t_rng_generator);
            uncover_nearby(t_cell, uncover_result.uncovered_cells);
            if (!check_win_condition())
            {
                break;
            }
            restart();
        }
        m_game_state = GameState::PLAYING;
        return uncover_result;
    }
    const auto index = cell_to_index(t_cell);
    auto& cell = m_cells[index];
    if (cell == CellState::FLAGGED)
    {
        return uncover_result;
    }
    if (cell == CellState::COVERED)
    {
        uncover_result.action = UncoverAction::UNCOVER;
        uncover_nearby(t_cell, uncover_result.uncovered_cells);
    }
    else if (cell == CellState::UNCOVERED && can_uncover_nearby(t_cell))
    {
        uncover_result.action = UncoverAction::UNCOVER;
        for (int y_offset = -1; y_offset <= 1; ++y_offset)
        {
            for (int x_offset = -1; x_offset <= 1; ++x_offset)
            {
                const glm::vec2 nearby_cell = glm::ivec2{t_cell} + glm::ivec2{x_offset, y_offset};
                if (is_valid_position(nearby_cell) && m_cells[cell_to_index(nearby_cell)] == CellState::COVERED)
                {
                    uncover_nearby(nearby_cell, uncover_result.uncovered_cells);
                }
            }
        }
    }
    if (m_game_state == GameState::PLAYING && check_win_condition())
    {
        m_game_state = GameState::WON;
        m_tp_end = t_tp;
    }
    if (m_game_state == GameState::LOST)
    {
        uncover_result.action = UncoverAction::EXPLOSION;
        m_exploded_cell = t_cell;
        m_tp_end = t_tp;
    }
    return uncover_result;
}

void Minesweeper::generate(const glm::uvec2& t_cell, std::mt19937& t_rng_generator) noexcept
{
    m_bomb_cells.resize(m_bomb_amount, true);
    m_cells.resize(static_cast<std::size_t>(m_size.x) * m_size.y, CellState::COVERED);
    m_bomb_cells.resize(static_cast<std::size_t>(m_size.x * m_size.y) - 1, false);
    std::shuffle(m_bomb_cells.begin(), m_bomb_cells.end(), t_rng_generator);
    m_bomb_cells.insert(m_bomb_cells.begin() +
                            static_cast<decltype(m_bomb_cells)::difference_type>(t_cell.y) * m_size.x + t_cell.x,
                        false);
    for (unsigned y_grid = 0; y_grid < m_size.y; ++y_grid)
    {
        for (unsigned x_grid = 0; x_grid < m_size.x; ++x_grid)
        {
            std::uint_fast8_t nearby_amount{0};
            for (int y_offset = -1; y_offset <= 1; ++y_offset)
            {
                for (int x_offset = -1; x_offset <= 1; ++x_offset)
                {
                    const glm::uvec2 nearby_cell = glm::ivec2{x_grid, y_grid} + glm::ivec2{x_offset, y_offset};
                    if (is_valid_position(nearby_cell) && m_bomb_cells[cell_to_index(nearby_cell)])
                    {
                        ++nearby_amount;
                    }
                }
            }
            m_nearby_mines.emplace_back(nearby_amount);
        }
    }
}