#pragma once

class AudioDevice final
{
public:
    AudioDevice();
    ~AudioDevice() noexcept;

    AudioDevice(const AudioDevice&) = delete;
    AudioDevice(AudioDevice&&) = delete;
    AudioDevice& operator=(const AudioDevice&) = delete;
    AudioDevice& operator=(AudioDevice&&) = delete;
};