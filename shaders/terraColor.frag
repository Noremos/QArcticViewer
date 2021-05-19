#version 330 core
out vec4 FragColor;




in vec3 v_pos;
void main()
{
    vec3 lightPos = vec3(0,100,0);


//    vec3 coof = normalize(lightPos - v_pos);

//    vec3 color = (vec3(0.2,0.2,0.2) + coof) * vec3(1.0,1.0,1.0);
//    color = normalize(color);
//    FragColor = vec4(color,1.0);
    FragColor = vec4(0.7,0.7,0.7,1.0);
}
