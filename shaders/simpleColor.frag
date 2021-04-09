#version 150 core

//uniform sampler2D terrain;
//uniform vec4 ambient;
uniform sampler2D texture;

varying vec2 v_texcoord;

//! [0]
void main()
{
    // Set fragment color from texture
    gl_FragColor = texture2D(texture, v_texcoord);
//    gl_FragColor = vec4(0.4,0.6,1,1);
}
