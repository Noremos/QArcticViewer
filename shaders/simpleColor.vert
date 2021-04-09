#version 150 core

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;


attribute vec4 a_position;
attribute vec2 a_texcoord;


varying vec2 v_texcoord;

//! [0]
void main()
{
    gl_Position = projection * view * model  * a_position;//vec4(a_position, 1.0);
    v_texcoord = a_texcoord;
}
