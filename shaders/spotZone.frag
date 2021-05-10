#version 330 core

in float outColor;
out vec4 fragColor;
uniform float opacity;

void main(void)
{
    vec3 colors[4];
    colors[0] = vec3(0.0, 1.0, 0.0);
    colors[1] = vec3(0.54, 0.0, 0.5);
    colors[2] = vec3(0.0, 0.3, 0.7);
    colors[3] = vec3(1.0, 0.0, 0.0);

    int type = int(round(outColor)) - 1;

    fragColor = vec4(colors[type], opacity);
}
