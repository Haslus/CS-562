#version 450 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D EDTex;
layout(binding = 1) uniform sampler2D sceneTex;

uniform vec2 ScreenSize;


void main()
{   
	float offset_x = 1.0 / ScreenSize.x;
	float offset_y = 1.0 / ScreenSize.y;

	vec2 offsets[9] = 
	{
	vec2(-offset_x,offset_y),
	vec2(0,offset_y),
	vec2(offset_x,offset_y),

	vec2(-offset_x,0),
	vec2(0,0),
	vec2(offset_x,0),

	vec2(-offset_x,-offset_y),
	vec2(0,-offset_y),
	vec2(offset_x,-offset_y)
	
	};

	vec3 color = vec3(0,0,0);

	for(int i = 0; i < 9; i++)
	{
		vec2 position = TexCoords + offsets[i] * texture(EDTex,TexCoords).x;
		color += texture(sceneTex, position).rgb;
	}

	color += texture(sceneTex, TexCoords).rgb;

	color.r /= 9;
	color.g /= 9;
	color.b /= 9;

    FragColor = vec4(color,1);
}