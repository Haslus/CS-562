#version 450 core

out vec4 FragColor;

layout(binding = 0) uniform sampler2D sceneTex;

in vec2 TexCoords;


void main()
{   
	float brightness = dot(texture(sceneTex,TexCoords).rgb, vec3(0.2126, 0.7152, 0.0722));

	FragColor = vec4(texture(sceneTex,TexCoords).rgb,1.0) * brightness;

}