

in float outColor;
out vec4 fragColor;
uniform float opacity;
//uniform int allowType;

void main(void)
{
    vec3 colors[7];
    colors[1] = vec3(0.0, 1.0, 0.0);
    colors[2] = vec3(0.54, 0.0, 0.5);
    colors[3] = vec3(0.0, 0.3, 0.7);
    colors[4] = vec3(1.0, 0.0, 0.0);
    colors[5] = vec3(0.58, 0.29, 0.0);
    colors[6] = vec3(1.0, 0.5, 0.0);

    int type = int(round(outColor));

    fragColor = vec4(colors[type], opacity);
}
