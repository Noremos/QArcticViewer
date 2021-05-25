#version 330 core

vec3 mainColor = vec3(0.8, 0.8, 0.8);

out vec4 fragColor;
in float curHei;
in vec2 v_texcoord;


float rand(float n){return fract(sin(n) * 43758.5453123);}

float noise(float p)
{
    float fl = floor(p);
    float fc = fract(p);
    return mix(rand(fl), rand(fl + 1.0), fc);
}

//float noise(vec2 n)
//{
//    const vec2 d = vec2(0.0, 1.0);
//    vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
//    return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
//}

void main(void)
{
//    vec3 noised = vec3(noise(v_texcoord.x), noise(curHei), noise(v_texcoord.y));
    vec3 noised = vec3(noise(curHei * 100));
    fragColor = vec4(mix(mainColor, noised, 0.5), 1.0); //color;
}
