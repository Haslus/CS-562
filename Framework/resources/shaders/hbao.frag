#version 450

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D Normal;
layout(binding = 1) uniform sampler2D Position;
layout(binding = 2) uniform sampler2D Noise;

#define PI 3.1415926535897932384626433832795f

uniform float radius = 3;
uniform float angleBias = 0;
uniform int numDirections = 4;
uniform int numSteps = 10;
uniform float attenuation = 1;
uniform float scale = 1;
uniform vec2 ScreenSize;

float angle_of_2_vectors(vec3 v1, vec3 v2)
{
	return acos( dot(v1,v2) );
}

void main()
{
	vec3 currentP = texture(Position,TexCoords).xyz;

	float offset = radians(360.0f / numDirections);

	float projected_radius = float(radius);

	float stepSize = float(projected_radius / numSteps);

	vec2 Tstep = 1.0f / ScreenSize;

	float totalAO = 0;

	float randomFactor = radians(90 * texture(Noise,TexCoords).r);

	for(int i = 0; i < numDirections; i++)
	{
		vec2 dir = normalize( vec2( cos(offset * i + randomFactor), sin(offset * i + randomFactor) ) );

		float tangentAngle = radians(90) - angle_of_2_vectors(normalize(texture(Normal,TexCoords).xyz), vec3(dir,0)) + radians(angleBias);

		float horizonAngle = 0;
		float distance = 0;
		vec2 sampleOffset = dir * stepSize * Tstep;

		for(int j = 1; j <= numSteps; j++)
		{
			vec2 currentTC = TexCoords + j * sampleOffset;

			//Horizon Vector
			vec3 H =  normalize(texture(Position,currentTC).xyz - currentP);

			//float angleH = atan( H.z / length(H.xy));
			float angleH =  radians(90) - angle_of_2_vectors(H,normalize(texture(Normal,TexCoords).xyz));

			if(length(H) < radius)
			{
				if( horizonAngle < angleH )
				{
					horizonAngle = angleH;
					distance = length(texture(Position,currentTC).xyz - currentP);

				}
			}	

		}

		float falloff = clamp(0.0f,1.0f,1.0f - distance * distance);
		float AO = (sin(horizonAngle) - sin(tangentAngle)) * attenuation * falloff;

		totalAO += AO;
	}


	totalAO /= numDirections;
	totalAO = 1 - totalAO;


	FragColor = vec4(totalAO * scale,totalAO * scale,totalAO * scale,1);

}