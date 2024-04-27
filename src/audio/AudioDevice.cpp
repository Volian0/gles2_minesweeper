#include "AudioDevice.hpp"

#include <SDL_mixer.h>

#include <stdexcept>

AudioDevice::AudioDevice()
{
    // open the default audio device
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) != 0)
    {
        throw std::runtime_error("Failed to open audio device!");
    }
}
AudioDevice::~AudioDevice() noexcept
{
    Mix_CloseAudio();
}