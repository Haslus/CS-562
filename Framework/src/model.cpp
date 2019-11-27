/**
* @file		model.cpp
* @date 	02/08/2019
* @author	Asier Bilbao
* @par		Login: asier.b
* @par		Course: CS 562
* @par		Assignment #1
* @brief 	Framework
*
*/

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

#include "pch.h"
#include "model.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glsc2ext.h"

#include <assimp/cimport.h>

std::vector<Texture> textures_loaded;

/**
* @brief 	Custom destructor for the Mesh, also creates the data for Rendering
*/
Mesh::~Mesh()
{
	glDeleteVertexArrays(1,&VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}
/**
* @brief 	Custom constructor for the Mesh, also creates the data for Rendering
*/
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
	std::vector<Texture> textures, Material material)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->material = material;

	SetupMesh();
}
/**
* @brief 	Setup a mesh with vertices and indices
*/
void Mesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//Vertex Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	//Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));
	//Vertex Texture Coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_texcoords));
	//Vertex Tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_tangent));
	//Vertex Bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_bitangent));

	glBindVertexArray(0);

}

/**
* @brief 	Draws the mesh using the giving shader
*/
void Mesh::Draw(Shader shader, bool wireframe, bool tessellation)
{
	//Set textures
	for (unsigned i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string tex_name = textures[i].m_type;

		shader.SetInt(tex_name, i);
		glBindTexture(GL_TEXTURE_2D, textures[i].m_id);

	}
	//Set material properties
	shader.SetFloat("diffuse", material.diffuse.x != 0 ? material.diffuse.x : 0);
	shader.SetFloat("specular", material.specular.x != 0 ? material.specular.x : 0);
	shader.SetFloat("shininess", material.shininess != 0 ? material.shininess : 0);

	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(VAO);
	if (tessellation)
	{
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawElements(GL_PATCHES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	}
	else
	{

		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);

}
/**
* @brief 	Custom constructor of Model, loads the model
*/
Model::Model(const std::string& path)
{
	LoadModel(path);
}

/**
* @brief 	Draw the mesh of the model
* @param	shad
*/
void Model::Draw(Shader shader, bool tessellation)
{
	shader.SetMat4("model",this->transform.M2W);

	for (auto mesh : m_meshes)
		mesh->Draw(shader, wireframe, tessellation);
}

/**
* @brief 	Loads the actual model from a given path
*/
void Model::LoadModel(const std::string& path)
{
	Assimp::Importer import;
	
	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipUVs );

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR ASSIMP " << aiGetErrorString() << std::endl;
		return;
	}

	m_directory = path.substr(0, path.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);
}
/**
* @brief 	Processes the nodes inside the scene
*/
void Model::ProcessNode(aiNode * node, const aiScene * scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}

}
/**
* @brief 	Processes the meshes inside the scene
*/
Mesh* Model::ProcessMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	//For each vertex in the mesh
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		glm::vec3 vert, norm, tan, bitan;
		glm::vec2 texcoord(0,0);
		//Store position
		vert.x = mesh->mVertices[i].x;
		vert.y = mesh->mVertices[i].y;
		vert.z = mesh->mVertices[i].z;

		vertex.m_position = vert;
		//Store normals
		norm.x = mesh->mNormals[i].x;
		norm.y = mesh->mNormals[i].y;
		norm.z = mesh->mNormals[i].z;

		vertex.m_normal = norm;
		if (mesh->mTangents)
		{
			//Store tangents
			tan.x = mesh->mTangents[i].x;
			tan.y = mesh->mTangents[i].y;
			tan.z = mesh->mTangents[i].z;
			vertex.m_tangent = tan;

			//Store bitangents
			bitan.x = mesh->mBitangents[i].x;
			bitan.y = mesh->mBitangents[i].y;
			bitan.z = mesh->mBitangents[i].z;
			vertex.m_bitangent = bitan;
		}
		

		

		//Check for textures
		if (mesh->mTextureCoords[0])
		{
			texcoord.x = mesh->mTextureCoords[0][i].x;
			texcoord.y = mesh->mTextureCoords[0][i].y;
		}
		//Store texture coordinates
		vertex.m_texcoords = texcoord;

		vertices.push_back(vertex);
	}
	//Store indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);

	}

	//Process material
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	aiColor4D specular;
	aiColor4D diffuse;
	aiColor4D ambient;
	float shininess;

	aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular);
	aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
	aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient);
	aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess);

	//Load Diffuse, Specular and Normal textures

	bool skip = false;
	aiString str;
	material->GetTexture(aiTextureType_DIFFUSE, 0, &str);

	Texture texture;
	if (TextureFromFile(str.C_Str(), texture.m_id))
	{
		texture.m_type = "Diffuse";
		texture.m_path = str.C_Str();
		textures.push_back(texture);
		textures_loaded.push_back(texture);
	}
	
	
	return new Mesh(vertices, indices, textures, Material{ specular,diffuse,ambient,shininess });
}

/**
* @brief 	Loads the a texture from the given path
*/

bool TextureFromFile(const char * path,unsigned int & textureID)
{
	std::string filename = std::string(path);
	//filename = "./data" + filename.substr(filename.find_first_of('.') + 1);

	int width, height, comps;
	
	auto i = loadDDS(filename.c_str(), width, height);

	if (i)
	{
		textureID = i;
		/*glBindTexture(GL_TEXTURE_2D, i);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/


	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		//stbi_image_free(data);
		return false;
	}


	return true;
}

Decal::Decal(const std::string & diffuse, const std::string & normal, Model * model)
{
	cube = model;
	Texture diffTexture;
	if (TextureFromFile(diffuse.c_str(), diffTexture.m_id))
	{
		diffTexture.m_type = "Diffuse";
		diffTexture.m_path = diffuse.c_str();
		textures.push_back(diffTexture);
		textures_loaded.push_back(diffTexture);
	}

	Texture normalTexture;
	if (TextureFromFile(normal.c_str(), normalTexture.m_id))
	{
		normalTexture.m_type = "NormalMap";
		normalTexture.m_path = normal.c_str();
		textures.push_back(normalTexture);
		textures_loaded.push_back(normalTexture);
	}
}

void Decal::Draw(Shader shader, DrawMode drawMode)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[0].m_id);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[1].m_id);
	shader.SetInt("drawMode", drawMode);
	switch (drawMode)
	{
	case Decal::FULLDECAL:
	{
		cube->Draw(shader);
		break;
	}
	case Decal::ONLYPIXELS:
	{
		cube->Draw(shader);
		break;
	}
	case Decal::SHADING:
	{
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_ONE,GL_ZERO,GL_ZERO,GL_ONE);
		cube->Draw(shader);
		glDisable(GL_BLEND);
		break;
	}
	default:
		break;
	}
}

GLuint loadDDS(const char * imagepath, int & _width, int & _height) 
{

	unsigned char header[124];

	FILE *fp;

	/* try to open the file */
	fp = fopen(imagepath, "rb");
	if (fp == NULL)
		return 0;

	/* verify the type of file */
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		fclose(fp);
		return 0;
	}

	/* get the surface desc */
	fread(&header, 124, 1, fp);

	unsigned int height = *(unsigned int*)&(header[8]);
	unsigned int width = *(unsigned int*)&(header[12]);
	unsigned int linearSize = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC = *(unsigned int*)&(header[80]);

	_width = width;
	_height = height;

	unsigned char * buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);
	/* close the file pointer */
	fclose(fp);
	
	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch (fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		return 0;
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
	{
		unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
			0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if (width < 1) width = 1;
		if (height < 1) height = 1;

	}

	free(buffer);

	return textureID;
}