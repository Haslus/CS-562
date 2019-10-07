#version 450 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D normalTex;
layout(binding = 1) uniform sampler2D depthTex;

const float offset = 1.0 / 300.0;
const float normal_factor = 0.05;
const float depth_factor = 0.25;

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

	float sobel_x[9] = 
	{
	1,0,-1,
	2,0,-2,
	1,0,-1
	};

	float sobel_y[9] = 
	{
	1,2,1,
	0,0,0,
	-1,-2,-1
	};

	float normal_Dx = 0;
	float normal_Dy = 0;

	float depth_Dx = 0;
	float depth_Dy = 0;

	for(int i = 0; i < 9; i++)
	{
		depth_Dx += texture(depthTex, TexCoords + offsets[i]).x * sobel_x[i];
		depth_Dy += texture(depthTex, TexCoords + offsets[i]).y * sobel_y[i];

		normal_Dx += texture(normalTex, TexCoords + offsets[i]).x * sobel_x[i];
		normal_Dx += texture(normalTex, TexCoords + offsets[i]).y * sobel_y[i];
	}

	float normal_gradient = sqrt(normal_Dx * normal_Dx + normal_Dy * normal_Dy);
	float depth_gradient = sqrt(depth_Dx * depth_Dx + depth_Dy * depth_Dy);
	float gradient = normal_gradient * normal_factor+ depth_gradient * depth_factor; 
	float blur_factor = clamp(gradient,0,1);

    FragColor = vec4(blur_factor,blur_factor,blur_factor,1);
}