#version 400
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in mat3 TBN;

uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform sampler2D NormalMap;

uniform float diffuse;
uniform float specular;
uniform float shininess;

void main()
{
	//Store data on the G-Buffer
    gPosition = vec4(FragPos,0);

	vec3 norm = texture(NormalMap,TexCoords).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	gNormal = vec4(normalize(TBN * norm),0);

	gAlbedoSpec = texture(Diffuse, TexCoords);

	//Ignore if alpha is too low
	if(gAlbedoSpec.a < 0.5f)
	{
		discard;
	}

	gAlbedoSpec.a = texture(Specular, TexCoords).r * specular + 1;

	//Also store in the G-Buffer the material's properties
	gPosition.a = diffuse;
	gNormal.a = shininess;
}
