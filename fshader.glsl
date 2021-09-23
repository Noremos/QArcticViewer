

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform sampler2D texture0;

in vec2 v_texcoord;
out vec4 color;
//! [0]
void main()
{
    // Set fragment color from texture
    color = texture(texture0, v_texcoord);
}
//! [0]

