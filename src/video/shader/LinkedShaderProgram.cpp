#include "LinkedShaderProgram.hpp"

LinkedShaderProgram::LinkedShaderProgram(std::span<const Attribute> t_attributes, std::string_view t_file_vertex,
                                         std::string_view t_file_fragment)
{
    bind_attributes_locations(t_attributes);
    link(t_file_vertex, t_file_fragment);
}