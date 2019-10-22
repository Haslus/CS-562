#version 400
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTexCoords;
layout(location = 2) in vec3 vNormal;

out vec3 Normal_vs;

uniform mat4 M; 
uniform mat4 V; 

void main()
{
    gl_Position = vec4(vPosition, 1.0);
    Normal_vs = vNormal;
}