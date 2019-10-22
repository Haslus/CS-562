#version 400 core

layout( location = 0 ) out vec4 FragColor;

in vec3 Normal_tes;

void main()
{ 
    FragColor = vec4(normalize(Normal_tes) + 1.0f / 2.0f,1);
}