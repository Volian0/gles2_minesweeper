#pragma once

#include "../video/Sprite.hpp"
#include "Button.hpp"
#include "GameState.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <optional>

class StateLevelSelect final : public GameState
{
public:
    enum class Buttons : std::uint_fast8_t
    {
        BACK,
        RESET_PB
    };

    struct LevelInfo final
    {
        enum class Difficulty : std::uint8_t
        {
            EASY,
            MEDIUM,
            HARD
        };
        Difficulty difficulty;
        glm::uvec2 size;
        unsigned bombs;
    };

    static constexpr Sprite EASY_TEXT{{32, 10}, {15, 5}, 2};
    static constexpr Sprite MEDIUM_TEXT{{32, 15}, {25, 5}, 2};
    static constexpr Sprite HARD_TEXT{{32, 20}, {15, 5}, 2};

    static constexpr glm::uvec2 LAYOUT_DIMENSION{3, 15};
    static constexpr glm::uvec2 LAYOUT_BOMBS{47, 15};
    static constexpr glm::uvec2 LAYOUT_PB{47, 22};

    static constexpr std::array<LevelInfo, 11> LEVELS{
        LevelInfo{LevelInfo::Difficulty::EASY, {10, 10}, 10},   LevelInfo{LevelInfo::Difficulty::EASY, {9, 9}, 10},
        LevelInfo{LevelInfo::Difficulty::EASY, {8, 8}, 10},     LevelInfo{LevelInfo::Difficulty::MEDIUM, {16, 16}, 40},
        LevelInfo{LevelInfo::Difficulty::MEDIUM, {16, 15}, 40}, LevelInfo{LevelInfo::Difficulty::MEDIUM, {15, 15}, 40},
        LevelInfo{LevelInfo::Difficulty::MEDIUM, {16, 14}, 40}, LevelInfo{LevelInfo::Difficulty::MEDIUM, {15, 14}, 40},
        LevelInfo{LevelInfo::Difficulty::MEDIUM, {16, 13}, 40}, LevelInfo{LevelInfo::Difficulty::MEDIUM, {15, 13}, 40},
        LevelInfo{LevelInfo::Difficulty::HARD, {30, 16}, 99}};

    StateLevelSelect(class Game* t_game) noexcept;

    void update();
    void handle_events();
    void draw_buttons();

    void read_high_scores();
    void reset_high_scores();

private:
    ButtonLayout m_button_layout_levels;
    ButtonLayout m_button_layout_bottom;

    bool m_back_clicked{false};
    std::optional<unsigned> m_selected_level;
    std::array<std::intmax_t, LEVELS.size()> m_high_scores{};
};