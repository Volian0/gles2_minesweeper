#pragma once

#include "../video/Sprite.hpp"
#include "Button.hpp"
#include "GameState.hpp"

class StateCredits final : public GameState
{
public:
    static constexpr Sprite SFX_CREDITS{{8, 112}, {42, 5}};
    static constexpr Sprite MUSIC_CREDITS{{0, 117}, {63, 5}};

    StateCredits(class Game* t_game);

    void update();

private:
    bool m_target_button{false};
    float m_layout_y_logo;
    float m_layout_y_credits;
    float m_layout_y_game;
    float m_layout_y_music;
    float m_layout_y_sfx;
    float m_layout_x_size;
    ButtonLayout m_button_layout;
    float m_layout_game_volian0_size;
    bool m_back_clicked{false};
};