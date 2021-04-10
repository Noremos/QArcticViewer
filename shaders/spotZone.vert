#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 instanceMatrix;

uniform int factor;
//uniform int minHei;
uniform mat4 projection;
uniform mat4 view;


void main()
{
    vec3 vp = aPos;
    vp.y *= factor;
//    vec2 offset = offsets[gl_InstanceID];
    //    gl_Position = vec4(aPos + offset, 0.0, 1.0);
    gl_Position = projection * view * instanceMatrix  * vec4(vp, 1.0);
}
