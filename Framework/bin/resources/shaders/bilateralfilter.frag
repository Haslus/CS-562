#version 450 core

#define PI 3.1415926535897932384626433832795

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D image;

uniform bool horizontal;

uniform float sigma_S = 1;
uniform float sigma_R = 1;

float calculate_kernel(float sigma, float value)
{
	return 1.0f / (2 * PI * sigma * sigma) *  exp( -(value * value) / (2 * sigma * sigma));
}

void main()
{   
	vec2 tex_offset = 1.0 / textureSize(image,0);
	vec3 I_p = texture(image, TexCoords).rgb * calculate_kernel(sigma_S, 0);

	vec3 BF_total = vec3(0);
	float WP_total = 0;

	if(horizontal)
	{
		for(int i = 1; i < 5; i++)
		{
			vec2 next_TC[2] = {TexCoords + vec2(tex_offset.x * i,0),
			TexCoords - vec2(tex_offset.x * i,0)};

			for(int j = 0; j < 2; j++)
			{
				vec3 I_q = texture(image, next_TC[j]).rgb;
				float G_s = calculate_kernel(sigma_S,length(TexCoords - next_TC[j]));
				float G_r = calculate_kernel(sigma_R,length(I_p - I_q));

				BF_total += G_s * G_r * I_q;
				WP_total += G_s * G_r;
			}
		}
	}
	else
	{
		for( int i = 1; i < 5; i++)
		{
			vec2 next_TC[2] = {TexCoords + vec2(0.0,tex_offset.y * i),
			TexCoords - vec2(0.0,tex_offset.y * i)};

			for(int j = 0; j < 2; j++)
			{
				vec3 I_q = texture(image, next_TC[j]).rgb;
				float G_s = calculate_kernel(sigma_S,length(TexCoords - next_TC[j]));
				float G_r = calculate_kernel(sigma_R,length(abs(I_p - I_q)));

				BF_total += G_s * G_r * I_q;
				WP_total += G_s * G_r;

			}
			

		}
	}


	BF_total /= WP_total;

	FragColor = vec4(BF_total,1.0);
}