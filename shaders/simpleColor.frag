#version 330 core

//uniform sampler2D terrain;
//uniform vec4 ambient;
uniform sampler2D texture0;

in vec2 v_texcoord;

out vec4 FragColor;
//! [0]
void main()
{
    // Set fragment color from texture
    FragColor = texture(texture0, v_texcoord);
//    FragColor = vec4(0.4,0.6,1,1);
}
