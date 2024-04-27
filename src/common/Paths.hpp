#pragma once

#include <string>

class Paths final
{
public:
    Paths(std::string_view t_organization, std::string_view t_application);

    [[nodiscard]] constexpr std::string base(std::string_view t_file) const noexcept
    {
        return m_base_path + t_file.data();
    }

    [[nodiscard]] constexpr std::string pref(std::string_view t_file) const noexcept
    {
        return m_pref_path + t_file.data();
    }

private:
    std::string m_base_path;
    std::string m_pref_path;
};