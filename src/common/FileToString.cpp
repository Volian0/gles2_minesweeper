#include "FileToString.hpp"

#include <fstream>
#include <sstream>

[[nodiscard]] std::string file_to_string(std::string_view t_file)
{
    std::stringstream buffer;
    buffer << std::ifstream{t_file.data()}.rdbuf();
    return buffer.str();
}