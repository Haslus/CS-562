#version 430
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 tcs_TexCoords;
out vec3 tcs_Normal;
out vec3 tcs_Tangent;
out vec3 tcs_Bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //vec4 worldPos = model * vec4(aPos, 1.0);

    //FragPos = worldPos.xyz;

    //gl_Position = projection * view * worldPos;

	gl_Position = vec4(aPos, 1.0);

    tcs_TexCoords = aTexCoords;
	tcs_Normal = aNormal;
	tcs_Tangent = aTangent;
	tcs_Bitangent = aBitangent;
}
