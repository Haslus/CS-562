#version 450

layout (location = 0) out vec4 gNormal;
layout (location = 1) out vec4 gAlbedoSpec;

in vec2 texCoords;

uniform vec2 screenSize;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int drawMode = 1;

uniform float angleLimit = 0;

layout(binding = 0) uniform sampler2D depthTex;
layout(binding = 1) uniform sampler2D diffuseTex;
layout(binding = 2) uniform sampler2D normalTex;

void main()
{          

	vec4 ndcPos;
	ndcPos.x = (2.0 * gl_FragCoord.x) / screenSize.x - 1.0;
	ndcPos.y = (2.0 * gl_FragCoord.y) / screenSize.y - 1.0;
	
	float z = texture(depthTex,texCoords).x;
	ndcPos.z = (2.0 * z - gl_DepthRange.near - gl_DepthRange.far) /
		(gl_DepthRange.far - gl_DepthRange.near);
	
	ndcPos.w = 1.0;
	
	vec4 clipPos = ndcPos / gl_FragCoord.w;
	vec4 viewPos = inverse(projection) * clipPos;
	vec4 worldPos = inverse(view) * viewPos;
	vec4 modelPos = inverse(model) * worldPos;
	
	//Check if it is inside the box, discard otherwise
	if(modelPos.x < -0.5 || modelPos.x > 0.5)
		discard;
	if(modelPos.y < -0.5 || modelPos.y > 0.5)
		discard;

	if(drawMode == 0)
	{
		gNormal = texture(normalTex, texCoords);
		gAlbedoSpec = vec4(1,1,1,1);
	}
	else
	{
		//Check if normal is valid respect to the angle
		//vec3 givenNormal = texture(normalTex, texCoords).xyz;
	
		//if( atan(givenNormal.y,givenNormal.x) > angleLimit)
			//discard;
	
		//Use model coordinate as texture coordinate
		vec2 decTexCoords = modelPos.xy + 0.5;
	
		//Sample textures
		vec4 diffuseSample = texture(diffuseTex, decTexCoords);
		vec4 normalSample = texture(normalTex, decTexCoords);
	
	
		//Normal needs to be on tangent space
		vec3 dxWp = dFdx(worldPos).xyz;
		vec3 dyWp = dFdx(worldPos).xyz;
	
		vec3 normal = normalize(cross(dxWp,dyWp));
		vec3 tangent = normalize(dyWp);
		vec3 bitangent = normalize(dxWp);
	
		vec3 T =  normalize(vec3(model * vec4(tangent,  0.0)));
		vec3 B = -normalize(vec3(model * vec4(bitangent,0.0)));
		vec3 N =  normalize(vec3(model * vec4(normal,   0.0)));
		mat3 TBN = mat3(T, B, N);
	
		vec3 finalNormal = normalize(TBN * normalSample.xyz);

		gNormal = vec4(finalNormal,1);
		gAlbedoSpec = vec4(diffuseSample);
	}
	
	
   

}