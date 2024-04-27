#include "Music.hpp"

#include <stdexcept>

Music::Music(std::string_view t_file) : m_music{Mix_LoadMUS(t_file.data())}
{
    if (m_music == nullptr)
    {
        throw std::runtime_error("Failed to load music!");
    }
}
Music::~Music() noexcept
{
    Mix_FreeMusic(m_music);
}
void Music::play()
{
    if (Mix_PlayMusic(m_music, 0) != 0)
    {
        throw std::runtime_error("Failed to play music!");
    }
}
void Music::fade_in(float t_seconds)
{
    if (Mix_FadeInMusic(m_music, 0, static_cast<int>(t_seconds * MILLISECONDS_IN_SECOND)) != 0)
    {
        throw std::runtime_error("Failed to fade in music!");
    }
}
void Music::fade_out(float t_seconds)
{
    Mix_FadeOutMusic(static_cast<int>(t_seconds * MILLISECONDS_IN_SECOND));
}