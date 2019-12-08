#version 450 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 vert_TexCoords;
out vec3 vert_Normal;

void main()
{
	vert_TexCoords = aTexCoords;
	vert_Normal = aNormal;
	gl_Position = vec4(aPosition,1);
}