#version 150 core
in vec3 vertexPosition;

out vec2 TexCoord;
out float curHei;

uniform mat4 mvp_matrix;

uniform float factor;

void main()
{
    vec3 vp = vertexPosition;

    curHei = vp.y;
    vp.y += factor;


    gl_Position = mvp_matrix * vec4(vp, 1.0);
}
