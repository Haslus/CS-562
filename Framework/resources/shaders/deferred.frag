#version 430
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 AmbientBuffer;
layout (location = 4) out vec4 gLinearDepth;
layout (location = 5) out vec4 gRefinementDepth;

in vec3 finalNormal;
in vec3 finalTangent;
in vec3 finalBitangent;
in vec2 finalTexCoords;
in vec3 finalPos;

in float refinementDepth;

uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform sampler2D NormalMap;

uniform float diffuse;
uniform float specular;
uniform float shininess;
uniform float ambient;

uniform float zNear = 0.1;
uniform float zFar = 1000.0;

uniform mat4 model;

vec4 HSVtoRGB(float hue)
{
	float H = mix(270.0,0.0,hue);

	float X = (1 - abs( mod( H / 60.0,2.0) - 1));

	vec3 value;

	if( 0.0 <= H && H < 60.0)
	{
		value = vec3(1,X,0);
	}
	
	else if( 60.0 <= H && H < 120.0)
	{
		value = vec3(X,1,0);
	}
	
	else if( 120.0 <= H && H < 180.0)
	{
		value = vec3(0,1,X);
	}

	else if( 180.0 <= H && H < 240.0)
	{
		value = vec3(0,X,1);
	}

	else if( 240.0 <= H && H < 300.0)
	{
		value = vec3(X,0,1);
	}
	else if( 300.0 <= H && H < 360.0)
	{
		value = vec3(1,0,X);
	}
	

	return vec4(value,1);
}


void main()
{
	//Store data on the G-Buffer
    gPosition = vec4(finalPos,0);

	//Create matrix for tangent space
	vec3 T = normalize(vec3(model * vec4(finalTangent,   0.0)));
	vec3 B = -normalize(vec3(model * vec4(finalBitangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(finalNormal,    0.0)));
    mat3 TBN = mat3(T, B, N);

	vec3 norm = texture(NormalMap,finalTexCoords).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	gNormal = vec4(normalize(TBN * norm),0);

	gAlbedoSpec = texture(Diffuse, finalTexCoords);

	//Ignore if alpha is too low
	if(gAlbedoSpec.a < 0.5f)
	{
		discard;
	}

	gAlbedoSpec.a = texture(Specular, finalTexCoords).r * specular + 1;

	//Also store in the G-Buffer the material's properties
	gPosition.a = diffuse;
	gNormal.a = shininess;

	AmbientBuffer = texture(Diffuse, finalTexCoords) * ambient;

	float depth = gl_FragCoord.z  * 2.0 - 1.0;
	float linear_depth = (2.0 * zNear * zFar) / (zFar + zNear - depth * (zFar - zNear));
	linear_depth /= zFar;
	gLinearDepth = vec4(linear_depth,linear_depth,linear_depth,1);

	gRefinementDepth = HSVtoRGB(refinementDepth);
}
