#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 instanceMatrix; //vec[0]
//     (location = 2)                         //vec[1]
//     (location = 3)                         //vec[2]
//     (location = 4)                         //vec[3]
layout (location = 5) in float aColor;

uniform int factor;
uniform float minval;

uniform mat4 projection;
uniform mat4 view;

out float outColor;

void main()
{
    outColor = aColor;
    //aColor;
    vec3 vp = aPos;
    vp.y += (vp.y - minval) * factor;
//    vec2 offset = offsets[gl_InstanceID];
    //    gl_Position = vec4(aPos + offset, 0.0, 1.0);
    gl_Position = projection * view * instanceMatrix  * vec4(vp, 1.0);
}
