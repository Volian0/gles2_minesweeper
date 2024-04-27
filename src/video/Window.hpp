#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

#include <string_view>

class Window final
{
public:
    Window(std::string_view t_title, const glm::uvec2& t_size);
    ~Window() noexcept;

    [[nodiscard]] constexpr auto ptr() noexcept
    {
        return m_ptr;
    }

    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;

private:
    SDL_Window* m_ptr;
};