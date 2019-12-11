#version 450 core

out vec4 FragColor;

in vec2 textureMesh;
in vec2 textureFin;
in float out_opacity;


layout(binding = 0) uniform sampler2D Diffuse;
layout(binding = 1) uniform sampler2D finTexture;
layout(binding = 2) uniform sampler2D finOffset;

void main()
{
	vec4 finOpacity = texture(finTexture,textureFin);
	vec4 offset = texture(finOffset,textureFin);

	vec2 texCoords = textureMesh;
	texCoords -= (offset.xy - 0.5f) * 2.0f;
	vec4 color = texture(Diffuse,texCoords);

	color.a = finOpacity.a * out_opacity;

	FragColor =color.xxxa;
}