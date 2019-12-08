#version 450
out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D Diffuse;
layout(binding = 1) uniform sampler2D Scene;

void main()
{

	FragColor = vec4(texture(Scene,TexCoords).rgb,1);
}
