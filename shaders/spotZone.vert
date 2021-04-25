#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 instanceMatrix; //vec[0]
//     (location = 2)                         //vec[1]
//     (location = 3)                         //vec[2]
//     (location = 4)                         //vec[3]
layout (location = 5) in float aColor;

uniform float minHei;
uniform int factor;

uniform mat4 projection;
uniform mat4 view;

out float outColor;

void main()
{
    outColor = aColor;
    //aColor;
    vec3 vp = aPos;
    mat4 rmat = instanceMatrix;

        vp.y =  -1 + (vp.y + 1) * factor;
//    vp.y += factor;
//    vec2 offset = offsets[gl_InstanceID];
    //    gl_Position = vec4(aPos + offset, 0.0, 1.0);
//    rmat[1][1] *= factor;.

    //x  s 0 0 t
    //z  0 s 0 t
    //y  0 0 s t
    //w  0 0 0 1
   rmat[3][1] = minHei + (rmat[3][1] - minHei) * factor;

    gl_Position = projection * view * rmat  * vec4(vp, 1.0);
}
