//uniform sampler2D qt_Texture0;
//varying vec4 qt_TexCoord0;

//void main(void)
//{
//    gl_FragColor = texture2D(qt_Texture0, qt_TexCoord0.st);
//}

#version 330 core

uniform sampler2D texture0;

in vec2 v_texcoord;

out vec4 FragColor;
//! [0]
void main()
{
    // Set fragment color from texture
    FragColor = texture(texture0, v_texcoord);
//    if (FragColor == vec4(1.0,1.0,1.0,1.0))
//        FragColor.a = 0;
//    FragColor = vec4(0.4,0.6,1,1);
}
