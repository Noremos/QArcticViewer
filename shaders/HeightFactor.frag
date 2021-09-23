

in float curHei;

out vec4 fragColor;

vec3 fstColor = vec3(0, 0, 1);
vec3 secColor = vec3(1, 0.35, 0);

//float minHei = -2.36199;
//float maxHei = 10.919;
//float minHei = -1.09489;
//float maxHei = -0.085615;//9,93035

uniform float minHei;// = -2.1988;
uniform float maxHei;// = 3.18347;


void main(void)
{
    float dff = (curHei - minHei) / (maxHei - minHei);

    fragColor = vec4(mix(fstColor, secColor, dff),1.0); //color;
}
