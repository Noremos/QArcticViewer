#version 150 core
in vec3 vertexPosition;

out vec2 TexCoord;
out float y;

uniform mat4 modelMatrix;
uniform mat4 mvp;

void main()
{
    y = vertexPosition.z;

//    position = vec3(modelView * vec4(vertexPosition, 1.0));

    // Transform position, normal, and tangent to world coords
    // Calculate vertex position in clip coordinates
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
