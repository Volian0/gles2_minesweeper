#include "Context.hpp"

#include "Window.hpp"

#include <stdexcept>

Context::Context(Window& t_window) : m_context{SDL_GL_CreateContext(t_window.ptr())}
{
    if (m_context == nullptr)
    {
        throw std::runtime_error("Failed to create context!");
    }
    // try to turn on adaptive VSync
    if (SDL_GL_SetSwapInterval(-1) != 0)
    {
        // if it fails, try to turn on normal VSync
        SDL_GL_SetSwapInterval(1);
    }
}
Context::~Context() noexcept
{
    SDL_GL_DeleteContext(m_context);
}