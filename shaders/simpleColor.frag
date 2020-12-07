#version 150 core

uniform sampler2D terrain;
uniform vec4 ambient;

in vec2 TexCoord;
out vec4 fragColor;

void main(void)
{
    fragColor = texture(terrain, TexCoord);
//    fragColor = ambient;
}

