
layout(location = 0) in vec3 a_position;

out float curHei;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform float minHei;// = -2.1988;
uniform int factor;

void main()
{
    curHei = a_position.y;

    +calcPos()
}
