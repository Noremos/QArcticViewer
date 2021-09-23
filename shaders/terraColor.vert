

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
    v_texcoord = a_texcoord;

    +calcPos()
}
