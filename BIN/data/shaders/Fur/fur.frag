#version 450 core

out vec4 FragColor;

layout(binding = 0) uniform sampler2D Diffuse;
layout(binding = 1) uniform sampler2DArray furTextureArray;
layout(binding = 2) uniform sampler2DArray furOffsetArray;


const float textureTilingFactor = 10.0; 

in vec2 outTexCoords;
in flat int shellNumber;

void main()
{           

	vec4 outputColor;

	vec2 TexCoords = outTexCoords * textureTilingFactor;
	vec4 tangentAndAlpha = texture(furTextureArray,vec3(TexCoords, shellNumber));
	vec4 offset = texture(furOffsetArray,vec3(TexCoords, shellNumber));

	TexCoords -= (offset.xy - 0.5f) * 2.0f;
	TexCoords /= textureTilingFactor;
	outputColor = texture(Diffuse, TexCoords);

	if(outputColor.a < 0.01)
		discard;

	outputColor.a = tangentAndAlpha.a * offset.a; 

	FragColor = outputColor;

}