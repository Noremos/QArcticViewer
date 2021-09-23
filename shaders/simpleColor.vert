

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform float minHei;// = -2.1988;
uniform int factor;

out vec2 v_texcoord;
//! [0]
void main()
{
    v_texcoord = a_texcoord;

    +calcPos()
}
