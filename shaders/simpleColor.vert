#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform float minHei;// = -2.1988;
uniform int factor;

out vec2 v_texcoord;
out vec3 v_pos;
//! [0]
void main()
{
    mat4 rmat = model;
    vec3 vp = a_position;
    v_pos = vp;
//    vp.y *= factor;

//    rmat[1][1] *= factor;

    vp.y = minHei + (vp.y - minHei) * factor;
    gl_Position = projection * view * rmat  * vec4(vp, 1.0);

    v_texcoord = a_texcoord;
}
