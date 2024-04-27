#version 100

precision mediump float;

uniform sampler2D u_sampler;

varying vec2 v_uv;
varying vec3 v_tint;

void main()
{
    vec4 color = texture2D(u_sampler, v_uv);
    if (color.a < 0.5)
    {
        discard;
    }
    gl_FragColor = color * vec4(v_tint, 1.0);
}