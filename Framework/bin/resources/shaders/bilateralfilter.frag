#version 450 core

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D image;

uniform float sigmaS = 1;
uniform float sigmaL = 1;


void main()
{   
	float facS = -1.0 / (2.0 * sigS * sigS);
	float facL = -1.0 / (2.0 * sigL * sigL);

	float sumW = 0.0;
	vec4 sumC = vec4(0.0);
	float halfSize = sigS * 2;
	vec2 texSize = textureSize(texture,0);

	float l = length(texture(image,TexCoords).xyz);

	for(float i = -halfSize; i <= halfSize; i++)
	{
		for(float j = -halfSize; j <= halfSize; j++)
		{
			vec2 pos = vec2(i,j);
			vec4 offsetColor = texture2D(image, TexCoords + pos / textureSize);

			float distS = length(pos);
			float distL = length(offsetColor.xyz) - 1;
			
			float wS = exp(facS * distS * distS );
			float wL = exp(facL * distL * distL );

			float w = wS*wL;

			sumW += w;
			sumC += offsetColor * w;

		}
	}

	FragColor = sumC / sumW;
}