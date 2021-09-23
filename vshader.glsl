
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texcoord;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

out vec2 v_texcoord;

//! [0]
void main()
{
    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = a_texcoord;

    // Calculate vertex position in screen space
    gl_Position = projection * view * model  * a_position;

}
//! [0]
