
layout (location = 0) in vec3 a_position;
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

    //x  s 0 0 t
    //z  0 s 0 t
    //y  0 0 s t
    //w  0 0 0 1

    mat4 model = instanceMatrix;
    +calcPos()
}
