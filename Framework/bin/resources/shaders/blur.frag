#version 450 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D EDTex;
layout(binding = 1) uniform sampler2D sceneTex;

const float offset = 1.0 / 300.0;

void main()
{           
	vec2 offsets[9] = 
	{
	vec2(-offset,offset),
	vec2(0,offset),
	vec2(offset,offset),

	vec2(-offset,0),
	vec2(0,0),
	vec2(offset,0),

	vec2(-offset,-offset),
	vec2(0,-offset),
	vec2(offset,-offset)
	
	};

	vec3 color = vec3(0,0,0);

	for(int i = 0; i < 9; i++)
	{
		vec2 position = TexCoords + offsets[i] * texture(EDTex,TexCoords).x;
		color += texture(sceneTex, position).rgb;
	}

	color += texture(sceneTex, TexCoords).rgb;

	color.r /=9;
	color.g /=9;
	color.b /=9;

    FragColor = vec4(color,1);
}