#version 450

out vec4 FragColor;

in vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D ZBuffer;
uniform sampler2D Normals;
uniform sampler2D Position;

uniform float radius = 30;
uniform float angleBias = 0;
uniform int numDirections = 4;
uniform int numSteps = 10;
uniform float attenuation = 1;
uniform float scale = 1;
uniform vec2 ScreenSize;

void main()
{

	float offset = 360.0f / numDirections;
	int totalSteps = radius / numSteps;

	vec2 Tstep = 1.0 / ScreenSize;

	float totalAO = 0;

	for(int i = 0; i < numDirections; i++)
	{
		vec2 dir = normalize( vec2( cos(offset * i), sin(offset * i) ) );

		vec2 TC = TexCoords;
		vec2 H_XY;

		float z = 0;
		for(int j = 0; j < numSteps; j++0)
		{
			TC += dir * totalSteps;
			float Zsample = ZBuffer(TC);

			if(z < Zsample)
			{
				z = Zsample;
				H_XY = TC;
			}

		}
		vec3 H = normalize(vec3(TC,z));

		vec3 T = normalize(texture(Position,TC + Tstep) - texture(Position, TC - Tstep));

		float angleH = atan( H.z / length(H.xy));

		float angleT = atan( T.z / length(T.xy));

		float AO = sin(angleH) - sin(angleT);

		totalAO += AO;
	}


	totalAO /= numDirections;

	FragColor = totalAO;

}