#version 450
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D Diffuse;

uniform float Ambient;

void main()
{

	FragColor = texture(Diffuse,TexCoords) * Ambient;
}
