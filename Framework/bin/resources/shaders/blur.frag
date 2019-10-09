#version 450 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D EDTex;
layout(binding = 1) uniform sampler2D sceneTex;

uniform vec2 ScreenSize;

float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

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

	float g = texture(EDTex,TexCoords).x;

	if(g > 0.2)
	{
		for(int i = 0; i < 9; i++)
		{
			vec2 position = TexCoords + offsets[i];
			color += texture(sceneTex, position).rgb * kernel[i];
		}

		//color += texture(sceneTex, TexCoords).rgb;

		//color.r /= 10;
		//color.g /= 10;
		//color.b /= 10;

		FragColor = vec4(color,1);
	}

	else
	{
		color = texture(sceneTex, TexCoords).rgb;
		FragColor = vec4(color,1);
	}
	
	
}