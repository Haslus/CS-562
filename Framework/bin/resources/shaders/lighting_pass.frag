#version 450

out vec4 FragColor;

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gAlbedoSpec;
layout(binding = 3) uniform sampler2D Ambient;

uniform vec3 viewPos;
uniform bool HBAO;

struct Light {
    vec4 Position;
    vec4 Color;
    
	float Radius;
};

uniform Light light;
uniform vec2 ScreenSize;
//in vec2 TexCoords;
#define PI 3.14159265359

vec2 CalcTexCoords()
{
	return vec2(gl_FragCoord.x / ScreenSize.x, gl_FragCoord.y / ScreenSize.y) ;
}

void main()
{             
	//Get data from G-Buffer
	vec2 TexCoords = CalcTexCoords();
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a - 1;

	//Also get the material properties stored on the G-Buffer
	float Diffuse_material = texture(gPosition, TexCoords).a;
	float Shininess_material = texture(gNormal, TexCoords).a;
    
    //Calculate lighting
    vec3 lighting  = vec3(0,0,0);
    vec3 viewDir  = normalize(viewPos - FragPos);


	//Calculate distance between light source and current fragment
	float distance = length(light.Position.xyz - FragPos);

	//Calculate diffuse
	vec3 lightDir = normalize(light.Position.xyz - FragPos);
	vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color.rgb * Diffuse_material;

	//Calculate specular
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	float spec = pow(max(dot(Normal, halfwayDir), 0.0), Shininess_material);
	vec3 specular = light.Color.rgb * spec * Specular;

	//Apply attenuation
	float att = clamp(1.0 - distance * distance/(light.Radius * light.Radius),0,1);
	att *= att;
	diffuse *= att;
	specular *= att;
	lighting += diffuse + specular;
    
	
	if(HBAO)
		FragColor = vec4(lighting * texture(Ambient,TexCoords).r, 1.0);
	else
		FragColor = vec4(lighting, 1.0);


}