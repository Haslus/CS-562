#version 450 core
layout (triangles) in;

out vec2 TexCoords;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;


void main()
{
    gl_Position = proj * view * model * vec4(aPos, 1.0);
	TexCoords = aTexCoords; 
}