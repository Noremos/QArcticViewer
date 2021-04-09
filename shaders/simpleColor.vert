#version 330 core

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texcoord;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;


out vec2 v_texcoord;

//! [0]
void main()
{
    gl_Position = projection * view * model  * a_position;//vec4(a_position, 1.0);
    v_texcoord = a_texcoord;
}
