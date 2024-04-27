#version 100

uniform mat4 u_matrix;

attribute vec3 a_position;
attribute vec2 a_uv;
attribute vec3 a_tint;

varying vec2 v_uv;
varying vec3 v_tint;

void main()
{
    v_uv = a_uv;
    v_tint = a_tint;
    gl_Position = u_matrix * vec4(a_position, 1.0);
}