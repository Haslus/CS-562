#version 450 core

layout (triangles_adjacency) in;
layout (triangle_strip, max_vertices = 6) out; 

uniform vec3 Eye;
uniform float maxOpacity = 0.9;
uniform float finThreshold = 0.25;
uniform float combStrength = 0.3;
uniform vec3 combVector = vec3(0,0,1);
uniform float numShells = 15;
uniform float shellIncrement = 0.2;

const float EdgeWidth = 0.005f; // Width of sil. edge in clip cds.
const float PctExtend = 0.05f; // Percentage to extend quad

in vec2 vert_TexCoords[];
in vec3 vert_Normal[];

out vec2 textureMesh;
out vec2 textureFin;
out float out_opacity;

layout(binding = 0) uniform sampler2D Diffuse;

void MakeFin(float eyeDotN1, float eyeDotN2)
{
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
			gl_Position  = (gl_in[1].gl_Position + i * vec4(normalize(vert_Normal[0]) +
			combStrength*combVector,0)*numShells*shellIncrement*furLength[0]);
			textureFin = vec2(0,texcoord[i]);
			EmitVertex();
		}

		

		for(int i = 0; i < 2; i++)
		{
			textureMesh = vert_TexCoords[0];
			out_opacity = opacity;
			gl_Position  = (gl_in[0].gl_Position + i * vec4(normalize(vert_Normal[1]) +
			combStrength*combVector,0)*numShells*shellIncrement*furLength[1]);
			textureFin  = vec2(1,texcoord[i]);
			EmitVertex();
		}

		EndPrimitive();
	}
}

vec4 compute_face_normal(vec3 p0, vec3 p1, vec3 p2)
{
	vec3 v1 = p1-p0;
	vec3 v2 = p2-p0;
	return vec4(normalize(cross(v1,v2)),0.0);
}

void create_silhouette(vec3 e0, vec3 e1)
{
	const vec3 up = vec3(0,0,1);
	vec3 v = normalize(e1-e0);
	vec3 n = cross(v,up);
	
	vec4 e0_ = vec4(e0 - v*PctExtend, 1.0);
	vec4 e0_w = e0_ + vec4(EdgeWidth*n, 0.0);

	vec4 e1_ = vec4(e1 + v*PctExtend, 1.0);
	vec4 e1_w = e1_ + vec4(EdgeWidth*n, 0.0);

	gl_Position = e0_;
	EmitVertex();
	gl_Position = e0_w;
	EmitVertex();
	gl_Position = e1_w;
	EmitVertex();
	EndPrimitive();

	gl_Position = e0_;
	EmitVertex();
	gl_Position = e1_w;
	EmitVertex();
	gl_Position = e1_;
	EmitVertex();
	EndPrimitive();
}

void main()
{
   // Extract 6 vertices
    vec3 p0 = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
    vec3 p1 = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;
    vec3 p2 = gl_in[2].gl_Position.xyz / gl_in[2].gl_Position.w;
    vec3 p3 = gl_in[3].gl_Position.xyz / gl_in[3].gl_Position.w;
    vec3 p4 = gl_in[4].gl_Position.xyz / gl_in[4].gl_Position.w;
    vec3 p5 = gl_in[5].gl_Position.xyz / gl_in[5].gl_Position.w;
	
	// Compute face normals
	vec4 nT = compute_face_normal(p0,p2,p4);
	vec4 n0 = compute_face_normal(p0,p1,p2);
	vec4 n1 = compute_face_normal(p2,p3,p4);
	vec4 n2 = compute_face_normal(p4,p5,p0);
	
	// Discard if actual triangle normal is back facing
	if(nT.z <= 0)
		return;

	// Create silhouette if triangle is back facing
	if(n0.z <= 0)
		create_silhouette(p0,p2);
	if(n1.z <= 0)
		create_silhouette(p2,p4);
	if(n2.z <= 0)
		create_silhouette(p4,p0);
                             
}