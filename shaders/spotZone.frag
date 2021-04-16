#version 330 core

in float outColor;
out vec4 fragColor;

void main(void)
{
    int ds= 1;
    if (int(round(outColor))==ds)
        fragColor = vec4(0, 1, 0, 0.4);
    else
        fragColor = vec4(1, 0, 0, 0.4);

    // r g b
//   fragColor = vec4(1-outColor, outColor, 0, 0.4);
}
