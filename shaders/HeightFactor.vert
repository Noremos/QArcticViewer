#version 150 core
in vec3 vertexPosition;

out vec2 TexCoord;
out float curHei;

uniform mat4 modelMatrix;
uniform mat4 mvp;

uniform float factor;

void main()
{
    vec3 vp = vertexPosition;

    curHei = vp.y;
    vp.y *= factor;


    gl_Position = mvp * vec4(vp, 1.0);
}
