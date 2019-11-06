#version 450

layout (location = 0) out vec4 gNormal;
layout (location = 1) out vec4 gAlbedoSpec;

in vec2 texCoords;

uniform vec2 screenSize;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int drawMode = 0;

uniform float angleLimit = 0.8;

layout(binding = 0) uniform sampler2D depthTex;
layout(binding = 1) uniform sampler2D diffuseTex;
layout(binding = 2) uniform sampler2D normalTex;

void main()
{          

	vec2 fragCoords = gl_FragCoord.xy / screenSize;

	vec4 ndcPos;
	ndcPos.x = (2.0 * fragCoords.x - 1.0);
	ndcPos.y = (2.0 * fragCoords.y - 1.0);
	
	float z = texture(depthTex,fragCoords).x;
	ndcPos.z = (2.0 * z - 1.0);
	
	ndcPos.w = 1.0;
	
	vec4 viewPos = inverse(projection) * ndcPos;
	viewPos /= viewPos.w;
	vec4 worldPos = inverse(view) * viewPos;
	vec4 modelPos = inverse(model) * worldPos;


	if(drawMode == 0)
	{

		//Check if it is inside the box, discard otherwise
		if(modelPos.x < -0.5 || modelPos.x > 0.5)
			discard;
		if(modelPos.y < -0.5 || modelPos.y > 0.5)
			discard;
		if(modelPos.z < -0.5 || modelPos.z > 0.5)
			discard;

		//Use model coordinate as texture coordinate
		vec2 decTexCoords = modelPos.xy + vec2(0.5);

		//Sample textures
		vec4 diffuseSample = texture(diffuseTex, decTexCoords);

		if(diffuseSample.a < 0.5)
			discard;

		//Check if normal is valid respect to the angle
		vec3 givenNormal = texture(normalTex, decTexCoords).xyz;

		vec3 front = vec3(0,0,-1);

		front = normalize(transpose(inverse(mat3(model))) * front);

		//Normal needs to be on tangent space
		vec3 dxWp = dFdx(worldPos.xyz);
		vec3 dyWp = dFdy(worldPos.xyz);
	
		vec3 tangent = normalize(dxWp);
		vec3 bitangent = normalize(dyWp);

		vec3 normal = normalize(cross(tangent,bitangent));

		float angle = dot(front,normal);

		if(angle < angleLimit)
			discard;

	
		vec3 T = -normalize(vec3(tangent));
		vec3 B =  normalize(vec3(bitangent));
		vec3 N =  normalize(vec3(normal));
		mat3 TBN = mat3(T, B, N);
	
		vec3 finalNormal = normalize(TBN * ( 2.0 * givenNormal - 1));

		

		gNormal = vec4(finalNormal,1);
		gAlbedoSpec = vec4(diffuseSample);
	}
	else if(drawMode == 2)
	{
		gNormal = vec4(1,0,0,1);
		gAlbedoSpec = vec4(1,1,1,1);
	}
	else
	{
		//Check if it is inside the box, discard otherwise
		if(modelPos.x < -0.5 || modelPos.x > 0.5)
			discard;
		if(modelPos.y < -0.5 || modelPos.y > 0.5)
			discard;
		if(modelPos.z < -0.5 || modelPos.z > 0.5)
			discard;

		gNormal = vec4(1,0,0,1);
		gAlbedoSpec = vec4(1,1,1,1);
	}
   

}