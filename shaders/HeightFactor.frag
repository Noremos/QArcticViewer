#version 330 core

in float curHei;

out vec4 fragColor;

vec3 fstColor = vec3(0, 0, 1);
vec3 secColor = vec3(1, 0.35, 0);

//float minHei = -2.36199;
//float maxHei = 10.919;
//float minHei = -1.09489;
//float maxHei = -0.085615;//9,93035

uniform float aminHei;// = -2.1988;
uniform float amaxHei;// = 3.18347;


void main(void)
{
    vec3 dff = secColor - fstColor;// 1, -1, 0
    dff *= (curHei - aminHei)/ (amaxHei - aminHei); // (5 +2.36199) / (13) = 7/13 = 0.53 =
    dff += fstColor;

    fragColor = vec4(dff.rgb, 1.0); //color;
}
