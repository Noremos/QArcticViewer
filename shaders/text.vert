//attribute vec4 qt_Vertex;
//attribute vec4 qt_MultiTexCoord0;
//uniform mat4 qt_ModelViewProjectionMatrix;
//varying vec4 qt_TexCoord0;

//void main(void)
//{
//    gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
//    qt_TexCoord0 = qt_MultiTexCoord0;
//}

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 a_texcoord;
layout (location = 2) in mat4 instanceMatrix;

//uniform int minHei;
uniform mat4 projection;
uniform mat4 view;

out vec2 v_texcoord;
out vec2 offset;


void main()
{
//    vec2 offset = offsets[gl_InstanceID];
    //    gl_Position = vec4(aPos + offset, 0.0, 1.0);
    // coords of char location
    int index = int(round(instanceMatrix[3][3]));
    float xstart = float(index%8) / 63.0;
    float ystart = float(index/8) / 63.0;

    // offset/ tex.x max is 1. So 1/8 = max char len
    xstart += a_texcoord.x/8;
    ystart += a_texcoord.y/8;

    mat4 rightMatrix = instanceMatrix;
    rightMatrix[3][3] = 1.0;

    gl_Position = projection * view * rightMatrix  * vec4(aPos, 1.0);

    v_texcoord = vec2(xstart, ystart);
}
