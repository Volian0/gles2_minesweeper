#pragma once

#include <SDL.h>

class Context final
{
public:
    Context(class Window& t_window);
    ~Context() noexcept;

    Context(const Context&) = delete;
    Context(Context&&) = delete;
    Context& operator=(const Context&) = delete;
    Context& operator=(Context&&) = delete;

private:
    SDL_GLContext m_context;
};