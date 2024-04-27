#pragma once

#include "../audio/AudioDevice.hpp"
#include "../audio/Music.hpp"
#include "../audio/Sound.hpp"
#include "../common/Paths.hpp"
#include "../common/SDL.hpp"
#include "../video/Context.hpp"
#include "../video/QuadRenderer.hpp"
#include "../video/Sprite.hpp"
#include "../video/Texture.hpp"
#include "../video/Window.hpp"
#include "../video/shader/LinkedShaderProgram.hpp"
#include "Event.hpp"
#include "GameState.hpp"
#include "Transition.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <memory>
#include <random>

class Game final
{
public:
    enum class SoundType : std::uint_fast8_t
    {
        HOVER,
        FLAG,
        UNFLAG,
        UNCOVER,
        BOMB,
        SWOOSH1,
        SWOOSH2
    };

    enum class MusicType : std::uint_fast8_t
    {
        MENU,
        EASY,
        MEDIUM,
        HARD
    };

    static constexpr auto MUSIC_SIZE = 4;
    static constexpr auto SOUND_SIZE = 7;
    static constexpr unsigned MARGIN{4};
    static constexpr unsigned SMALL_MARGIN{2};
    static constexpr glm::vec2 ATLAS_SCALE{1.0F / glm::vec2{64.0F, 128.0F}};
    static constexpr glm::uvec2 INITIAL_WINDOW_SIZE{640, 360};

    static constexpr Sprite NUMBERS{{0, 0}, {3, 5}};
    static constexpr Sprite TIMER_ICON{{18, 10}, {5, 5}};
    static constexpr Sprite COLON_CHAR{{29, 5}, {1, 5}};
    static constexpr Sprite DOT_CHAR{{30, 5}, {1, 5}};
    static constexpr Sprite BUTTON{{0, 92}, {29, 9}, 1, 2};
    static constexpr Sprite BUTTON_TARGET{{29, 92}, {29, 9}};
    static constexpr Sprite LOGO{{0, 101}, {29, 11}, 2};
    static constexpr Sprite EXIT_TEXT{{32, 0}, {13, 5}};
    static constexpr Sprite RESET_TEXT{{32, 5}, {19, 5}};
    static constexpr Sprite CREDITS_TEXT{{36, 25}, {25, 5}, 1, 2};
    static constexpr Sprite BACK_TEXT{{48, 10}, {15, 5}};
    static constexpr Sprite GAME_TEXT{{1, 122}, {18, 5}, 1, 2};
    static constexpr Sprite VOLIAN_TEXT{{23, 122}, {26, 5}};
    static constexpr Sprite SLASH_CHAR{{26, 5}, {3, 5}};
    static constexpr Sprite X_CHAR{{32, 25}, {3, 5}};
    static constexpr Sprite BIG_BUTTON{{0, 32}, {56, 30}, 1, 3};
    static constexpr Sprite BIG_BUTTON_TARGET{{0, 62}, {56, 30}};

    static constexpr glm::vec3 COLOR_CLEAR{glm::vec3(0xc1, 0xc1, 0xd2) / float(0xff)};
    static constexpr glm::vec3 COLOR_TEXT2{glm::vec3(0xa6, 0xa6, 0xbf) / float(0xff)};
    static constexpr glm::vec3 COLOR_BLACK{glm::vec3(0x33, 0x33, 0x46) / float(0xff)};
    static constexpr glm::vec3 COLOR_GRAY{glm::vec3(0x53, 0x53, 0x73) / float(0xff)};
    static constexpr glm::vec3 COLOR_TEXT{glm::vec3(0x80, 0x80, 0xA4) / float(0xff)};

    static constexpr std::array<Attribute, 3> TINT3D_ATTRIBUTES{Attribute{"a_position", 3}, Attribute{"a_uv", 2},
                                                                Attribute{"a_tint", 3}};

    Game();

    template <typename T> void change_state(auto&&... t_args)
    {
        m_state_changed = true;
        m_state = std::make_unique<T>(this, std::forward<decltype(t_args)>(t_args)...);
        m_state->update_matrix();
    }

    [[nodiscard]] constexpr const glm::uvec2& get_window_size() const noexcept
    {
        return m_window_size;
    }

    [[nodiscard]] constexpr const glm::uvec2& get_mouse_position() const noexcept
    {
        return m_mouse_position;
    }

    [[nodiscard]] constexpr const glm::mat4& get_projection_matrix() const noexcept
    {
        return m_matrix_projection;
    }

    [[nodiscard]] constexpr auto get_delta_time() const noexcept
    {
        return m_delta_time;
    }

    [[nodiscard]] constexpr auto get_aspect_ratio() const noexcept
    {
        return m_aspect_ratio;
    }

    [[nodiscard]] constexpr const auto& get_events() const noexcept
    {
        return m_events;
    }

    [[nodiscard]] constexpr const auto& get_timepoint() const noexcept
    {
        return m_tp_new;
    }

    [[nodiscard]] constexpr auto& get_rng_generator() noexcept
    {
        return m_rng_generator;
    }

    template <SoundType SFX> void play_sfx()
    {
        m_sounds[static_cast<std::size_t>(SFX)].play(static_cast<unsigned>(SFX));
    }

    template <MusicType MusicToPlay> void play_music()
    {
        m_musics[static_cast<std::size_t>(MusicToPlay)].play();
    }

    template <MusicType MusicToPlay> void fade_in_music(float t_seconds)
    {
        m_musics[static_cast<std::size_t>(MusicToPlay)].fade_in(t_seconds);
    }

    [[nodiscard]] constexpr double get_elapsed_time() const noexcept
    {
        return m_elapsed_time;
    }

    [[nodiscard]] constexpr bool is_transitioning() const noexcept
    {
        return m_state_transition.is_transitioning();
    }

    [[nodiscard]] constexpr bool is_half_done() const noexcept
    {
        return m_state_transition.is_half_done();
    }

    [[nodiscard]] constexpr const auto& get_target_tint() const noexcept
    {
        return m_target_tint;
    }

    [[nodiscard]] constexpr const auto& get_paths() const noexcept
    {
        return m_paths;
    }

    void run();

    void stop();

    static std::string format_duration(const std::chrono::steady_clock::duration& t_duration);

    void write_number(glm::vec2 t_position, std::string_view t_number, const glm::vec3& t_tint);

    [[nodiscard]] double time_modulo(double t_seconds) const noexcept;

    void trigger_transition();

    void draw(const Sprite& t_sprite, const glm::vec2& t_position, const glm::vec3& t_tint = glm::vec3{1.0F},
              const glm::vec2& t_offset = glm::vec2{0.0F}, const glm::vec2& t_additional_size = glm::vec2{0.0F});

    void draw_advanced(const Sprite& t_sprite, const glm::vec2& t_position, float t_angle, const glm::vec2& t_origin,
                       const glm::vec3& t_tint = glm::vec3{1.0F}, float t_scale = 1.0F,
                       const glm::vec2& t_additional_size = glm::vec2{0.0F});

    void update_matrix(const glm::mat4& t_matrix);

private:
    void handle_events();
    void update_mouse_position();

    SDL m_sdl;
    Window m_window;
    Context m_gl_context;
    Paths m_paths;
    std::chrono::time_point<std::chrono::steady_clock> m_tp_old{std::chrono::steady_clock::now()};
    std::chrono::time_point<std::chrono::steady_clock> m_tp_new;
    std::unique_ptr<GameState> m_state;
    QuadRenderer m_quad_renderer;
    std::mt19937 m_rng_generator{std::random_device{}()};
    AudioDevice m_audio_device;
    std::array<Sound, SOUND_SIZE> m_sounds;
    std::array<Music, MUSIC_SIZE> m_musics;
    Texture m_texture_cell;
    LinkedShaderProgram m_shader;
    GLint m_uniform_matrix;
    float m_delta_time{0.0F};
    double m_elapsed_time{0.0};
    glm::uvec2 m_window_size{INITIAL_WINDOW_SIZE};
    glm::uvec2 m_mouse_position{};
    std::vector<Event> m_events;
    glm::vec3 m_target_tint{};
    glm::mat4 m_matrix_projection{1.0F};
    float m_aspect_ratio{float(INITIAL_WINDOW_SIZE.x) / float(INITIAL_WINDOW_SIZE.y)};
    Transition m_state_transition;
    bool m_state_changed = false;
};