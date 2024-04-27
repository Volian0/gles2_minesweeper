#pragma once

#include <stdexcept>
#include <string_view>

class StringSDL final
{
public:
    constexpr StringSDL(char* t_string) : m_string{t_string}
    {
        if (m_string == nullptr)
        {
            throw std::runtime_error("Problem with SDL string!");
        }
    }

    [[nodiscard]] constexpr std::string_view get() const noexcept
    {
        return m_string;
    }

    ~StringSDL() noexcept;

    StringSDL(const StringSDL&) = delete;
    StringSDL(StringSDL&&) = delete;
    StringSDL& operator=(const StringSDL&) = delete;
    StringSDL& operator=(StringSDL&&) = delete;

private:
    char* m_string;
};