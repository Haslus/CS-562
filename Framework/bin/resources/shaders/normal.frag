#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D Diffuse;
layout(binding = 1) uniform sampler2D Specular;
layout(binding = 2) uniform sampler2D Normal;

void main()
{           
    FragColor = texture(Diffuse, TexCoords);
	//FragColor = vec4(1,0,0,1);
}