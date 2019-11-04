#version 450

layout (location = 0) out vec4 gNormal;
layout (location = 1) out vec4 gAlbedoSpec;

in vec2 texCoords;

uniform vec2 screenSize;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int drawMode = 1;

uniform float angleLimit = 30;

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
		//Use model coordinate as texture coordinate
		vec2 decTexCoords = modelPos.xy + vec2(0.5);

		//Check if normal is valid respect to the angle
		vec3 givenNormal = texture(normalTex, decTexCoords).xyz;

		vec3 front = vec3(0,0,-1);

		front = transpose(inverse(mat3(view * model))) * front;

		float angle = dot(front,givenNormal);

		if(angle > angleLimit)
			discard;

		//Sample textures
		vec4 diffuseSample = texture(diffuseTex, decTexCoords);

		if(diffuseSample.a < 0.5)
			discard;
	
	
		//Normal needs to be on tangent space
		vec3 dxWp = dFdx(worldPos).xyz;
		vec3 dyWp = dFdy(worldPos).xyz;
	
		vec3 normal = normalize(cross(dxWp,dyWp));
		vec3 tangent = normalize(dyWp);
		vec3 bitangent = normalize(dxWp);
	
		vec3 T =  normalize(vec3(model * vec4(tangent,  0.0)));
		vec3 B = -normalize(vec3(model * vec4(bitangent,0.0)));
		vec3 N =  normalize(vec3(model * vec4(normal,   0.0)));
		mat3 TBN = mat3(T, B, N);
	
		//vec3 finalNormal = normalize(TBN * front);
		//givenNormal = normalize(TBN * givenNormal);

		

		gNormal = vec4(givenNormal,1);
		gAlbedoSpec = vec4(diffuseSample);
	}
	
	
   

}