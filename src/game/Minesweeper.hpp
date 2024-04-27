#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <random>
#include <ranges>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

class Minesweeper
{
public:
    enum class GameState : std::uint_fast8_t
    {
        NOT_STARTED,
        PLAYING,
        LOST,
        WON
    };
    enum class CellState : std::uint8_t
    {
        COVERED,
        FLAGGED,
        UNCOVERED,
    };

    enum class UncoverAction : std::uint_fast8_t
    {
        NOTHING,
        UNCOVER,
        EXPLOSION,
    };
    struct UncoverResult
    {
        UncoverAction action;
        std::vector<glm::uvec2> uncovered_cells;
    };
    enum class FlagAction : std::uint_fast8_t
    {
        NOTHING,
        FLAG,
        UNFLAG
    };

    constexpr Minesweeper(const glm::uvec2& t_size, unsigned t_bomb_amount) noexcept
        : m_size{t_size}, m_bomb_amount{t_bomb_amount}
    {
    }

    constexpr void restart() noexcept
    {
        *this = Minesweeper{m_size, m_bomb_amount};
    }

    [[nodiscard]] UncoverResult uncover(const glm::uvec2& t_cell,
                                        const std::chrono::time_point<std::chrono::steady_clock>& t_tp,
                                        std::mt19937& t_rng_generator) noexcept;

    [[nodiscard]] constexpr FlagAction flag(const glm::uvec2& t_cell) noexcept
    {
        if (!is_valid_position(t_cell) || m_game_state != GameState::PLAYING)
        {
            return FlagAction::NOTHING;
        }
        auto& cell = m_cells[cell_to_index(t_cell)];
        if (cell == CellState::UNCOVERED)
        {
            return FlagAction::NOTHING;
        }
        if (cell == CellState::COVERED)
        {
            cell = CellState::FLAGGED;
            ++m_flags_used;
            return FlagAction::FLAG;
        }
        cell = CellState::COVERED;
        --m_flags_used;
        return FlagAction::UNFLAG;
    }

    [[nodiscard]] constexpr bool can_target(const glm::uvec2& t_cell) const noexcept
    {
        if (!is_valid_position(t_cell) || m_game_state == GameState::LOST || m_game_state == GameState::WON)
        {
            return false;
        }
        if (m_game_state == GameState::NOT_STARTED)
        {
            return true;
        }
        return (m_cells[cell_to_index(t_cell)] != CellState::UNCOVERED || can_uncover_nearby(t_cell));
    }

    [[nodiscard]] constexpr auto get_used_flags() const noexcept
    {
        return m_flags_used;
    }

    [[nodiscard]] constexpr std::optional<std::chrono::steady_clock::duration> get_duration(
        const std::chrono::steady_clock::time_point& t_timepoint) const noexcept
    {
        if (m_game_state == GameState::PLAYING)
        {
            return t_timepoint - m_tp_start;
        }
        if (m_game_state == GameState::WON || m_game_state == GameState::LOST)
        {
            return m_tp_end - m_tp_start;
        }
        return std::nullopt;
    }

    [[nodiscard]] constexpr const auto& get_exploded_cell() const noexcept
    {
        return m_exploded_cell;
    }

    [[nodiscard]] constexpr auto get_game_state() const noexcept
    {
        return m_game_state;
    }

    [[nodiscard]] constexpr const auto& size() const noexcept
    {
        return m_size;
    }

    [[nodiscard]] constexpr bool is_bomb(const glm::uvec2& t_cell) const noexcept
    {
        return m_bomb_cells[cell_to_index(t_cell)];
    }

    [[nodiscard]] constexpr CellState& get_cell_state(const glm::uvec2& t_cell) noexcept
    {
        return m_cells[cell_to_index(t_cell)];
    }

    [[nodiscard]] constexpr std::uint8_t get_nearby_mines(const glm::uvec2& t_cell) const noexcept
    {
        return m_nearby_mines[cell_to_index(t_cell)];
    }

    [[nodiscard]] constexpr auto get_bomb_amount() const noexcept
    {
        return m_bomb_amount;
    }

private:
    [[nodiscard]] constexpr bool is_valid_position(const glm::uvec2& t_cell) const noexcept
    {
        return t_cell.x < m_size.x && t_cell.y < m_size.y;
    }

    [[nodiscard]] constexpr std::size_t cell_to_index(const glm::uvec2& t_cell) const noexcept
    {
        return t_cell.y * m_size.x + t_cell.x;
    }

    [[nodiscard]] constexpr bool can_uncover_nearby(const glm::uvec2& t_cell) const noexcept
    {
        std::uint_fast8_t nearby_flags{};
        bool covered_cells{false};
        for (int y_offset = -1; y_offset <= 1; ++y_offset)
        {
            for (int x_offset = -1; x_offset <= 1; ++x_offset)
            {
                const glm::uvec2 nearby_cell = glm::ivec2{t_cell} + glm::ivec2{x_offset, y_offset};
                if (is_valid_position(nearby_cell))
                {
                    const auto cell = m_cells[cell_to_index(nearby_cell)];
                    if (cell == CellState::FLAGGED)
                    {
                        ++nearby_flags;
                    }
                    else if (cell == CellState::COVERED)
                    {
                        covered_cells = true;
                    }
                }
            }
        }
        return covered_cells && nearby_flags != 0 && nearby_flags == m_nearby_mines[cell_to_index(t_cell)];
    }

    constexpr void uncover_nearby(const glm::uvec2& t_cell, std::vector<glm::uvec2>& t_uncovered_cells) noexcept
    {
        const auto force_uncover = [&](const glm::uvec2& t_cell) {
            auto& cell = m_cells[cell_to_index(t_cell)];
            t_uncovered_cells.emplace_back(t_cell);
            if (cell == CellState::FLAGGED)
            {
                --m_flags_used;
            }
            cell = CellState::UNCOVERED;
            ++m_uncovered_cells;
        };
        const auto index = cell_to_index(t_cell);
        if (m_cells[index] == CellState::UNCOVERED)
        {
            return;
        }
        if (m_bomb_cells[index])
        {
            m_game_state = GameState::LOST;
            for (unsigned y_grid = 0; y_grid < m_size.y; ++y_grid)
            {
                for (unsigned x_grid = 0; x_grid < m_size.x; ++x_grid)
                {
                    if (m_bomb_cells[y_grid * m_size.x + x_grid])
                    {
                        force_uncover({x_grid, y_grid});
                    }
                }
            }
            return;
        }
        force_uncover(t_cell);
        if (m_nearby_mines[index] == 0)
        {
            for (int y_offset = -1; y_offset <= 1; ++y_offset)
            {
                for (int x_offset = -1; x_offset <= 1; ++x_offset)
                {
                    const glm::uvec2 nearby_cell = glm::ivec2{t_cell} + glm::ivec2{x_offset, y_offset};
                    if (is_valid_position(nearby_cell))
                    {
                        uncover_nearby(nearby_cell, t_uncovered_cells);
                    }
                }
            }
        }
    };

    void generate(const glm::uvec2& t_cell, std::mt19937& t_rng_generator) noexcept;

    glm::uvec2 m_size;
    unsigned m_bomb_amount;
    std::vector<bool> m_bomb_cells;
    std::vector<std::uint8_t> m_nearby_mines;
    std::vector<CellState> m_cells;
    unsigned m_flags_used{0};
    unsigned m_uncovered_cells{0};
    glm::uvec2 m_exploded_cell{};
    std::chrono::time_point<std::chrono::steady_clock> m_tp_start;
    std::chrono::time_point<std::chrono::steady_clock> m_tp_end;
    GameState m_game_state{GameState::NOT_STARTED};
};