#include "StringSDL.hpp"

#include <SDL.h>

StringSDL::~StringSDL() noexcept
{
    SDL_free(m_string);
}