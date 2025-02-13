#version 150 core
in vec3 vertexPosition;

uniform mat4 modelMatrix;
uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
