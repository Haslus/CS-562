#version 450 core

out vec4 FragColor;

const float textureTilingFactor = 10.0; 

struct VS_Out_Scene
{
	vec4 Position;
	vec3 Normal;
	vec2 TexCoords;
	vec3 LightVector;
	vec3 ViewVector;
	vec3 CombVector;

};

in flat int shellNumber;
in VS_Out_Scene VSScene;

layout(binding = 0) uniform sampler2D Diffuse;
layout(binding = 1) uniform sampler2DArray furTextureArray;
layout(binding = 2) uniform sampler2DArray furOffsetArray;

uniform int shell = 0;
uniform vec3 combVector = vec3(0,0,1);
uniform float combStrength = 0;
uniform int numShells = 16;
uniform float shellIncrement = 0.005;

void main()
{           
	float ka = 0.3;
	float kd = 0.7;
	float ks = 0.2;
	float specPower = 20.0;

	vec4 outputColor;

	vec2 TexCoords = VSScene.TexCoords * textureTilingFactor;
	vec4 tangentAndAlpha = texture(furTextureArray,vec3(TexCoords, shellNumber),0);
	vec4 offset = texture(furOffsetArray,vec3(TexCoords, shellNumber),0);

	TexCoords -= (offset.xy - 0.5f) * 2.0f;
	TexCoords /= textureTilingFactor;
	outputColor = texture(Diffuse, TexCoords);

	if(outputColor.a < 0.01)
		discard;


	outputColor.a = tangentAndAlpha.a * offset.a; 

	//lighting
	vec3 lightVector = normalize(VSScene.LightVector);
	vec3 viewVector = normalize(VSScene.ViewVector);
	
	vec3 tangentVector = normalize((tangentAndAlpha.rgb - 0.5f) * 2.0f);
	tangentVector = normalize(tangentVector + VSScene.CombVector);
	
	float TdotL = dot(tangentVector, lightVector);
	float TdotE = dot(tangentVector, viewVector);
	
	float sinTL = sqrt(1 - TdotL * TdotL);
	float sinTE = sqrt(1 - TdotE * TdotE);
	
	//outputColor.xyz = ka * outputColor.xyz + kd * sinTL * outputColor.xyz +
	//ks * pow(abs(TdotL*TdotE + sinTL*sinTE ),specPower).xxx;
	//
	////self shadow
	//float minShadow = 0.8;
	//float shadow = (float(shellNumber) / float(numShells)) * (1-minShadow) + minShadow;
	//outputColor.xyz *= shadow;

	FragColor = outputColor;

}