#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D Diffuse;
layout(binding = 1) uniform sampler2D Specular;
layout(binding = 2) uniform sampler2D Normal;

void main()
{           
	vec2 TC = vec2(TexCoords.x,TexCoords.y);
    FragColor = texture(Diffuse, TC );
	//FragColor = vec4(TexCoords.x,TexCoords.y,0,1);
}