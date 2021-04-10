#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in mat4 instanceMatrix;


//uniform vec2 offsets[100];
uniform int factor;
uniform mat4 projection;
uniform mat4 view;

out float y;

void main()
{
    vec3 vp = aPos;
    vp.y *= factor;
    y = vp.y;
//    vec2 offset = offsets[gl_InstanceID];
    //    gl_Position = vec4(aPos + offset, 0.0, 1.0);
    gl_Position = projection * view * instanceMatrix  * vec4(aPos, 1.0);
}
