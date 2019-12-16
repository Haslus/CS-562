#version 450 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;


out int shellNumber;
out vec2 outTexCoords;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

uniform int shell = 0;
uniform vec3 combVector = vec3(0,0,1);
uniform float combStrength = 0;
uniform int numShells = 16;
uniform float shellIncrement = 0.005;
uniform float maxOpacity = 0.9;


layout(binding = 0) uniform sampler2D Diffuse;


void main()
{
	vec4 color = texture(Diffuse,aTexCoords);
	float lengthFraction = color.a;
    if(lengthFraction < 0.2) 
		lengthFraction = 0.2; 
		
	vec3 CombVector = combStrength*combVector;

	vec3 pos = aPosition.xyz + (aNormal + CombVector) * shellIncrement * shell * lengthFraction;

	outTexCoords = aTexCoords;

	shellNumber = shell - 1;

	gl_Position =  proj * view * model * vec4(pos,1);

}