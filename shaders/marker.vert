
layout (location = 0) in vec3 a_position;
//layout (location = 1) in float aColor;

uniform float minHei;
//uniform float maxHei;
uniform float localMinHei;
uniform int factor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out float outColor;

void main()
{
    outColor = 1;
    //aColor;
    vec3 vp = a_position;
    mat4 rmat = model;

    //x  s 0 0 t
    //z  0 s 0 t
    //y  0 0 s t
    //w  0 0 0 1
   rmat[3][1] = rmat[3][1] + (rmat[3][1] - minHei) * factor;

    gl_Position = projection * view * rmat  * vec4(vp, 1.0);
}
