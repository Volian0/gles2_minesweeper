#pragma once

class SDL final
{
public:
    SDL();
    ~SDL() noexcept;

    SDL(const SDL&) = delete;
    SDL(SDL&&) = delete;
    SDL& operator=(const SDL&) = delete;
    SDL& operator=(SDL&&) = delete;
};