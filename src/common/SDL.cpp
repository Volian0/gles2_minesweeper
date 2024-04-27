#include "SDL.hpp"

#include <SDL.h>

#include <stdexcept>

SDL::SDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        throw std::runtime_error("Failed to initialize SDL!");
    }
}
SDL::~SDL() noexcept
{
    SDL_Quit();
}