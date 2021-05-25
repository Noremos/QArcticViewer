#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord;

out float curHei;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform float minHei;// = -2.1988;
uniform int factor;

out float curHei;

out vec2 v_texcoord;

void main()
{
    vec3 vp = a_position;
    curHei = vp.y;
    vp.y = minHei + (vp.y - minHei) * factor;

    gl_Position =  projection * view * model * vec4(vp, 1.0);

    v_texcoord = a_texcoord;
}
