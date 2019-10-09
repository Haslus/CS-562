#version 450 core

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D scene;
layout(binding = 1) uniform sampler2D bloomBlur;

void main()
{   
	const float gamma = 2.2;
	vec3 sceneColor = texture(scene, TexCoords).rgb;
	vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
	sceneColor += bloomColor;

	FragColor = vec4(sceneColor, 1.0);
}