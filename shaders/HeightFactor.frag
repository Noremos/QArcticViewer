#version 150 core

in float curHei;
in float outFactor;

out vec4 fragColor;

vec3 fstColor = vec3(0, 0, 1);
vec3 secColor = vec3(1, 0.35, 0);

//float minHei = -2.36199;
//float maxHei = 10.919;
float minHei = -1.09489;
float maxHei = -0.085615;//9,93035
uniform float factor;

void main(void)
{
    vec3 dff = secColor - fstColor;// 1, -1, 0
    dff *= abs((curHei - minHei)/ (maxHei - minHei)); // (5 +2.36199) / (13) = 7/13 = 0.53 =
    dff += fstColor;

    fragColor = vec4(dff.rgb, 1.0); //color;
}
