#version 150 core

//layout (location = 0) in vec3 vertexPosition;
//layout (location = 1) in vec3 vertexNormal;
//layout (location = 2) in vec2 vertexTexCoord;

in vec3 vertexPosition;
//in vec3 vertexNormal;
in vec2 vertexTexCoord;
//in vec4 vertexTangent;

//out vec3 position;
out vec2 TexCoord;

uniform mat4 modelMatrix;
uniform mat4 mvp;
uniform float factor;


void main()
{
    vec3 t = vec3(vertexTexCoord, 1.0);

    TexCoord = (t / t.z).xy;
    vec3 vp = vertexPosition;
    vp.y*= factor;
//    position = vec3(modelView * vec4(vertexPosition, 1.0));

    // Transform position, normal, and tangent to world coords
    // Calculate vertex position in clip coordinates
    gl_Position = mvp * vec4(vp, 1.0);
}
