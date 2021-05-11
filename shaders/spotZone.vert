#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 instanceMatrix; //vec[0]
//     (location = 2)                         //vec[1]
//     (location = 3)                         //vec[2]
//     (location = 4)                         //vec[3]
layout (location = 5) in float aColor;

uniform float minHei;
uniform float localMinHei;
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

    //x  s 0 0 t
    //z  0 s 0 t
    //y  0 0 s t
    //w  0 0 0 1
   rmat[3][1] = minHei + (rmat[3][1] - minHei) * factor;
   vp.y = localMinHei + (vp.y - localMinHei) * factor;

    gl_Position = projection * view * rmat  * vec4(vp, 1.0);
}
