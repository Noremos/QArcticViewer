#version 330 core

flat in int outColor;
out vec4 fragColor;

void main(void)
{
    if (outColor==1)
        fragColor = vec4(0, 1, 0, 0.4);
    else
        fragColor = vec4(1, 0, 0, 0.4);
}
