#version 450 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;


struct VS_Out_Scene
{
	vec4 Position;
	vec3 Normal;
	vec2 TexCoords;
	vec3 LightVector;
	vec3 ViewVector;
	vec3 CombVector;

};

out int shellNumber;
out VS_Out_Scene VSScene;


uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;


uniform vec3 Eye;
uniform vec3 Light;

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

	VSScene.Position = proj * view * model * vec4(pos,1);
	VSScene.Normal = normalize( vec3(model * vec4(aNormal,1)));
	VSScene.TexCoords = aTexCoords;

	vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
	vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
	mat3 TBN = mat3(T, B, N);

	//Transform light and eye vector to tangent space
	vec3 eyeVector = Eye - pos;
	VSScene.ViewVector = eyeVector;

	vec3 lightVector = Light - pos;
	VSScene.LightVector = lightVector;

	//transform the comb vector aswell, since this is going to be needed for 
    //transforming the fur tangent in the lighting calculations
    VSScene.CombVector =  combVector;

	shellNumber = shell - 1;

	gl_Position =  proj * view * model * vec4(pos,1);

}