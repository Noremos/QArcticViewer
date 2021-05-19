#version 330 core
in vec3 vertexPosition;

out float curHei;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform float minHei;// = -2.1988;
uniform int factor;

void main()
{
    vec3 vp = vertexPosition;
    curHei = vp.y;
    vp.y = minHei + (vp.y - minHei) * factor;

    gl_Position =  projection * view * model * vec4(vp, 1.0);
}
