#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform int factor;
uniform float minval;

out vec2 v_texcoord;

//! [0]
void main()
{
    vec3 vp = a_position;
    vp.y += (vp.y - minval) * factor;

    gl_Position = projection * view * model  * vec4(vp, 1.0);
    v_texcoord = a_texcoord;
}
