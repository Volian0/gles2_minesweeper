#include "Window.hpp"

#include <stdexcept>
#include <string_view>

Window::Window(std::string_view t_title, const glm::uvec2& t_size)
    : m_ptr([&]() {
          auto set_gl_attribute = [](auto t_attribute, auto t_value) {
              if (SDL_GL_SetAttribute(t_attribute, t_value) != 0)
              {
                  throw std::runtime_error("Failed to set OpenGL attribute!");
              }
          };
          set_gl_attribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
          set_gl_attribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
          set_gl_attribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
          return SDL_CreateWindow(t_title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  static_cast<int>(t_size.x), static_cast<int>(t_size.y),
                                  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
      }())
{

    if (m_ptr == nullptr)
    {
        throw std::runtime_error("Failed to create window!");
    }
}
Window::~Window() noexcept
{
    SDL_DestroyWindow(m_ptr);
}