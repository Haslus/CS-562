#version 400

out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform vec3 viewPos;
uniform float Ambient;
uniform vec2 ScreenSize;

struct Light {
    vec3 Position;
    vec3 Color;
    
	float Constant;
    float Linear;
    float Quadratic;
	float Radius;
};

uniform Light light;

vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / ScreenSize;
}


void main()
{             
	//Get data from G-Buffer
	vec2 TexCoords = CalcTexCoord();
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a - 1;
	//Also get the material properties stored on the G-Buffer
	float Diffuse_material = texture(gPosition, TexCoords).a;
	float Shininess_material = texture(gNormal, TexCoords).a;
    
    //Calculate lighting
    vec3 lighting  = Diffuse * Ambient;
    vec3 viewDir  = normalize(viewPos - FragPos);

	//Calculate distance between light source and current fragment
	float distance = length(lights.Position - FragPos);
	//Calculate diffuse
	vec3 lightDir = normalize(lights.Position - FragPos);
	vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights.Color * Diffuse_material;
	//Calculate specular
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	float spec = pow(max(dot(Normal, halfwayDir), 0.0), Shininess_material);
	vec3 specular = lights.Color * spec * Specular;
	//Apply attenuation
	float attenuation = 1.0 / (1.0 + lights.Linear * distance + lights.Quadratic * distance * distance);
	diffuse *= attenuation;
	specular *= attenuation;
	lighting += diffuse + specular;
     
	
    FragColor = vec4(lighting, 1.0);
}