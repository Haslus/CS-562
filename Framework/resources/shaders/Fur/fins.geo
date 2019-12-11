#version 450 core

layout (triangles_adjacency) in;
layout (triangle_strip, max_vertices = 12) out; 

uniform mat4 MVP;
uniform vec3 Eye;
uniform float maxOpacity = 0.9;
uniform float finThreshold = 0.25;
uniform float combStrength = 0.3;
uniform vec3 combVector = vec3(0,0,1);
uniform float numShells = 15;
uniform float shellIncrement = 0.005;

const float EdgeWidth = 0.005f; // Width of sil. edge in clip cds.
const float PctExtend = 0.05f; // Percentage to extend quad

in vec2 vert_TexCoords[];
in vec3 vert_Normal[];

out vec2 textureMesh;
out vec2 textureFin;
out float out_opacity;

layout(binding = 0) uniform sampler2D Diffuse;

void MakeFin(int idx1, int idx2)
{

	//const vec3 up = vec3(0,0,1);
	//vec3 v = normalize(e1-e0);
	//vec3 n = cross(v,up);
	//
	//vec4 e0_ = vec4(e0 - v*PctExtend, 1.0);
	//vec4 e0_w = e0_ + vec4(EdgeWidth*n, 0.0);
	//
	//vec4 e1_ = vec4(e1 + v*PctExtend, 1.0);
	//vec4 e1_w = e1_ + vec4(EdgeWidth*n, 0.0);

	float opacity = maxOpacity;

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
		gl_Position  = (gl_in[idx2].gl_Position + i * vec4(normalize(vert_Normal[idx2]) +
		combStrength*combVector,0)*numShells*shellIncrement*furLength[0]);
		textureFin = vec2(0,texcoord[i]);
		EmitVertex();
	}

		

	for(int i = 0; i < 2; i++)
	{
		textureMesh = vert_TexCoords[0];
		out_opacity = opacity;
		gl_Position  = (gl_in[idx1].gl_Position + i * vec4(normalize(vert_Normal[idx1]) +
		combStrength*combVector,0)*numShells*shellIncrement*furLength[1]);
		textureFin  = vec2(1,texcoord[i]);
		EmitVertex();
	}

	EndPrimitive();
	
}

vec4 compute_face_normal(vec3 p0, vec3 p1, vec3 p2)
{
	vec3 v1 = p1-p0;
	vec3 v2 = p2-p0;
	return vec4(normalize(cross(v1,v2)),0.0);
}

void create_silhouette(int e0, int e1, float opacity)
{
	vec2 texcoord = {1,0.1};
	float furLength[2];
	vec4 color = texture(Diffuse,vert_TexCoords[e0]);
	furLength[0] = color.a;
	color = texture(Diffuse,vert_TexCoords[e1]);
	furLength[1] = color.a;


	gl_Position = MVP * gl_in[e0].gl_Position;
	textureMesh = vert_TexCoords[e0];
	out_opacity = opacity;
	textureFin  = vec2(0,texcoord[0]);
	EmitVertex();

	gl_Position = MVP * (gl_in[e0].gl_Position + vec4(normalize(vert_Normal[e0]) + combStrength*combVector,0)*numShells*shellIncrement*furLength[0]);
	textureMesh = vert_TexCoords[e0];
	out_opacity = opacity;
	textureFin  = vec2(0,texcoord[1]);
	EmitVertex();

	gl_Position = MVP * gl_in[e1].gl_Position;
	textureMesh = vert_TexCoords[e1];
	out_opacity = opacity;
	textureFin  = vec2(1,texcoord[0]);
	EmitVertex();

	gl_Position = MVP * (gl_in[e1].gl_Position + vec4(normalize(vert_Normal[e1]) + combStrength*combVector,0)*numShells*shellIncrement*furLength[1]);
	textureMesh = vert_TexCoords[e1];
	out_opacity = opacity;
	textureFin  = vec2(1,texcoord[1]);
	EmitVertex();

	EndPrimitive();
}

void main()
{
   // Extract 6 vertices
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;
    vec3 p4 = gl_in[4].gl_Position.xyz;
    vec3 p5 = gl_in[5].gl_Position.xyz;

	vec3 eyeVec = normalize( Eye - p0 );
	
	
	// Compute face normals
	vec4 nT = compute_face_normal(p0,p2,p4);
	float eyeDotnT = dot(nT.xyz,eyeVec);


	// Create silhouette if triangle is back facing
	if(p1 != p0 && p1 != p2 && p1 != p4)
	{
		vec3 eyeVec1 = normalize( Eye - p0 );
		vec4 n0 = compute_face_normal(p0,p1,p2);
		float eyeDotn0 = dot(n0.xyz,eyeVec1);

			if(eyeDotn0 * eyeDotnT <= 0)
				create_silhouette(0,2,maxOpacity);

			else if(abs(eyeDotn0) < finThreshold)
				create_silhouette(0,2, (finThreshold - abs(eyeDotn0))*(maxOpacity/finThreshold));
		
		
	}

	if(p3 != p4 && p3 != p2 && p3 != p0)
	{
		vec3 eyeVec2 = normalize( Eye - p2 );
		vec4 n1 = compute_face_normal(p2,p3,p4);
		float eyeDotn1 = dot(n1.xyz,eyeVec2);

			if(eyeDotn1 * eyeDotnT <= 0)
				create_silhouette(2,4,maxOpacity);

			else if(abs(eyeDotn1) < finThreshold)
				create_silhouette(2,4, (finThreshold - abs(eyeDotn1))*(maxOpacity/finThreshold));
		
		
	}
	if(p5 != p4 && p5 != p0 && p5 != p2)
	{
		vec3 eyeVec3 = normalize( Eye - p4 );
		vec4 n2 = compute_face_normal(p4,p5,p0);
		float eyeDotn2 = dot(n2.xyz,eyeVec3);

		if(eyeDotn2 * eyeDotnT <= 0)
			create_silhouette(4,0,maxOpacity);

		else if(abs(eyeDotn2) < finThreshold)
			create_silhouette(4,0, (finThreshold - abs(eyeDotn2))*(maxOpacity/finThreshold));
		
	}
                             
}