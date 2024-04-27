#include "Paths.hpp"

#include "StringSDL.hpp"

#include <SDL.h>

Paths::Paths(std::string_view t_organization, std::string_view t_application)
    : m_base_path{StringSDL{SDL_GetBasePath()}.get()},
      m_pref_path{StringSDL{SDL_GetPrefPath(t_organization.data(), t_application.data())}.get()}
{
}