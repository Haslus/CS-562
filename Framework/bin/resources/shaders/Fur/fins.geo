#version 450 core

layout (triangles_adjacency ) in;
layout (triangle_strip, max_vertices = 4) out; 

uniform vec3 Eye;
uniform float maxOpacity = 0.9;
uniform float finThreshold = 0.25;
uniform float combStrength = 0.3;
uniform vec3 combVector = vec3(0,0,1);
uniform float numShells = 15;
uniform float shellIncrement = 0.2;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

in vec2 vert_TexCoords[];
in vec3 vert_Normal[];

out vec2 textureMesh;
out vec2 textureFin;
out float out_opacity;

layout(binding = 0) uniform sampler2D Diffuse;

void MakeFin(float eyeDotN1, float eyeDotN2)
{
	mat4 MVP = proj * view * model;
	float opacity = maxOpacity;
	bool makeFin = false;

	//If it is a silhouette edge
	if(eyeDotN1 * eyeDotN2 < 0)
	{
		makeFin = true;
		opacity = maxOpacity;
	}
	else if(abs(eyeDotN1) < finThreshold)
	{
		makeFin = true;
		opacity = (finThreshold - abs(eyeDotN1))*(maxOpacity/finThreshold);  
	}
	else if(abs(eyeDotN2) < finThreshold)
	{
		makeFin = true;
		opacity = (finThreshold - abs(eyeDotN2))*(maxOpacity/finThreshold);  
	}

	if(makeFin)
	{
		vec2 texcoord = {1,0.1};
		float furLength[2];
		vec4 color = texture(Diffuse,vert_TexCoords[0]);
		furLength[0] = color.a;
		color = texture(Diffuse,vert_TexCoords[1]);
		furLength[1] = color.a;

		

		for(int i = 0; i < 2; i++)
		{
			textureMesh = vert_TexCoords[1];
			out_opacity = opacity;
			gl_Position  = MVP * (gl_in[1].gl_Position + i * vec4(normalize(vert_Normal[0]) +
			combStrength*combVector,0)*numShells*shellIncrement*furLength[0]);
			textureFin = vec2(0,texcoord[i]);
			EmitVertex();
		}

		

		for(int i = 0; i < 2; i++)
		{
			textureMesh = vert_TexCoords[0];
			out_opacity = opacity;
			gl_Position  = MVP * (gl_in[0].gl_Position + i * vec4(normalize(vert_Normal[1]) +
			combStrength*combVector,0)*numShells*shellIncrement*furLength[1]);
			textureFin  = vec2(1,texcoord[i]);
			EmitVertex();
		}

		EndPrimitive();
	}
}

void main()
{
    vec3 eyeVec = normalize(Eye - vec3(gl_in[0].gl_Position));

	//compute triangles' normals

	vec3 triNormal1 = normalize(cross(vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position),
	vec3(gl_in[3].gl_Position) - vec3(gl_in[1].gl_Position)));

	float eyeDotN1 = dot(triNormal1,eyeVec);

	vec3 triNormal2 = normalize(cross(vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position),
	vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position)));

	float eyeDotN2 = dot(triNormal2,eyeVec);

    MakeFin(eyeDotN1,eyeDotN2);                                
}