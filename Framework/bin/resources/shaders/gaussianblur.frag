#version 450 core

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D image;

uniform bool horizontal;

uniform float weight[9];

uniform int size = 5;


void main()
{   
	vec2 tex_offset = 1.0 / textureSize(image,0);
	vec3 result = texture(image, TexCoords).rgb * weight[0];
	

	if(horizontal)
	{
		for(int i = 1; i < size; i++)
		{
			result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		}
	}
	else
	{
		for( int i = 1; i < size; i++)
		{
			result += texture(image, TexCoords + vec2(0.0,tex_offset.y * i)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(0.0,tex_offset.y * i)).rgb * weight[i];
		}
	}
	FragColor = vec4(result,1.0);
}