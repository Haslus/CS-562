#version 450

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout(binding = 0) uniform sampler2D ZBuffer;
layout(binding = 1) uniform sampler2D Normal;
layout(binding = 2) uniform sampler2D Position;
layout(binding = 3) uniform sampler2D Noise;

#define PI 3.1415926535897932384626433832795

uniform float radius = 30;
uniform float angleBias = 0;
uniform int numDirections = 4;
uniform int numSteps = 10;
uniform float attenuation = 1;
uniform float scale = 1;
uniform vec2 ScreenSize;

float angle_of_2_vectors(vec3 v1, vec3 v2)
{
	return acos(dot(v1,v2) / (length(v1) * length(v2) ) );
}

void main()
{

	float offset = 360.0f / numDirections;
	float stepSize = radius / numSteps;

	vec2 Tstep = 1.0 / ScreenSize;

	float totalAO = 0;

	float randomFactor = 1;

	for(int i = 0; i < numDirections; i++)
	{
		vec2 dir = normalize( vec2( cos(offset * randomFactor * i), sin(offset * randomFactor * i) ) );

		vec2 TC = TexCoords;

		vec3 B = normalize(cross(texture(Normal,TexCoords).xyz, vec3(dir,0) ));
		vec3 T = normalize(cross(texture(Normal,TexCoords).xyz,B));

		float tangentAngle = PI - angle_of_2_vectors(texture(Normal,TexCoords).xyz, vec3(dir,0));

		float horizonAngle = 0;

		for(int j = 0; j < numSteps; j++)
		{
			TC += dir * stepSize * Tstep; 

			//Horizon Vector
			vec3 H = texture(Position,TC).rgb - texture(Position,TexCoords).rgb;

			float angleH = atan( H.z / length(H.xy));

			if(horizonAngle < angleH )
			{
				horizonAngle = angleH;
			}

		}

		float AO = sin(horizonAngle) - sin(tangentAngle);

		totalAO += AO;
	}


	totalAO /= numDirections;
	totalAO = 1 - totalAO;
	FragColor = vec4(totalAO,totalAO,totalAO,1);

}