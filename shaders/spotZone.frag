#version 150 core

//uniform vec4 color;
//uniform int hei;
in float y;
out vec4 fragColor;


void main(void)
{
//    color.w *= 1.0 - (y / hei);
    fragColor = vec4(1.0,1.0,0,0.5); //color;
}
