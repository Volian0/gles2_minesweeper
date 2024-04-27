#pragma once

#include "../video/Sprite.hpp"
#include "Button.hpp"
#include "GameState.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <optional>

class StateMenu final : public GameState
{
public:
    enum class Buttons : std::uint_fast8_t
    {
        PLAY,
        CREDITS,
        EXIT
    };

    static constexpr Sprite PLAY_TEXT{{46, 0}, {15, 5}};
    static constexpr Sprite BY_TEXT{{50, 122}, {7, 5}};

    StateMenu(class Game* t_game, bool t_play_music = false);

    void update();

private:
    float m_layout_y_logo;
    float m_layout_x_size;
    glm::vec2 m_layout_first_button;
    ButtonLayout m_button_layout;
    glm::vec2 m_layout_game_text;

    std::optional<glm::vec2> m_target_button;
    std::optional<Buttons> m_clicked_button;
};