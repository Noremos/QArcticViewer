#version 330 core

in float outColor;
out vec4 fragColor;

void main(void)
{
    int ds = 1;
    int type = int(round(outColor));
    if (type==1)
        fragColor = vec4(0, 1, 0, 0.4);
    else if(type==2)
        fragColor = vec4(0.54, 0.0, 0.5, 0.4);
    else if(type==3)
        fragColor = vec4(0.0, 0.3, 0.7, 0.4);
    else
        fragColor = vec4(1, 0, 0, 0.4);

    // r g b
//   fragColor = vec4(1-outColor, outColor, 0, 0.4);
}
