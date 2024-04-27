#include "Sound.hpp"

#include <stdexcept>

Sound::Sound(std::string_view t_file) : m_chunk{Mix_LoadWAV(t_file.data())}
{
    if (m_chunk == nullptr)
    {
        throw std::runtime_error("Failed to load sound!");
    }
}

Sound::~Sound() noexcept
{
    Mix_FreeChunk(m_chunk);
}

void Sound::play(std::uint_fast8_t t_channel)
{
    if (Mix_PlayChannel(t_channel, m_chunk, 0) == -1)
    {
        throw std::runtime_error("Failed to play sound!");
    }
}