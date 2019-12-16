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
#include <map>
#include "model.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <assimp/cimport.h>

std::vector<Texture> textures_loaded;

unsigned int findAdjacentIndex(const aiMesh& mesh, const unsigned int index1, const unsigned int index2, const unsigned int index3) {

	for (unsigned int i = 0; i < mesh.mNumFaces; ++i) 
	{
		unsigned int*& indices = mesh.mFaces[i].mIndices;
		for (int edge = 0; edge < 3; ++edge) 
		{ //iterate all edges of the face
			unsigned int v1 = indices[edge]; 
			unsigned int v2 = indices[(edge + 1) % 3]; 
			unsigned int vOpp = indices[(edge + 2) % 3]; 
			
			if (((mesh.mVertices[v1] == mesh.mVertices[index1] && mesh.mVertices[v2] == mesh.mVertices[index2]) || 
				(mesh.mVertices[v2] == mesh.mVertices[index1] && mesh.mVertices[v1] == mesh.mVertices[index2])) 
				&& mesh.mVertices[vOpp] != mesh.mVertices[index3])
				return vOpp; //we have found the adjacent vertex
		}
	}

	//If it doesn't have an adjacent it's degenerate
	return index3;

}

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
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> tri_indices,
	std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->triangle_indices = tri_indices;
	this->textures = textures;

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangle_indices.size() * sizeof(unsigned int), &triangle_indices[0], GL_STATIC_DRAW);

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
void Mesh::Draw(Shader shader, bool wireframe, bool fins)
{

	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES_ADJACENCY, static_cast<GLsizei>(triangle_indices.size()), GL_UNSIGNED_INT, 0);
	
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

	//Build triangle adjacency list
	std::vector<unsigned int> true_idx;

	true_idx.resize(6 * mesh->mNumFaces);
	int index_off = 0;
	for (unsigned i = 0; i < mesh->mNumFaces; i++, index_off += 6)
	{
		true_idx[index_off    ] = mesh->mFaces[i].mIndices[0];
		true_idx[index_off + 2] = mesh->mFaces[i].mIndices[1];
		true_idx[index_off + 4] = mesh->mFaces[i].mIndices[2];
		true_idx[index_off + 1] = findAdjacentIndex(*mesh, true_idx[index_off + 0], true_idx[index_off + 2], true_idx[index_off + 4]);
		true_idx[index_off + 3] = findAdjacentIndex(*mesh, true_idx[index_off + 2], true_idx[index_off + 4], true_idx[index_off + 0]);
		true_idx[index_off + 5] = findAdjacentIndex(*mesh, true_idx[index_off + 4], true_idx[index_off + 0], true_idx[index_off + 2]);
	}
	

	return new Mesh(vertices, true_idx, textures);
}

/**
* @brief 	Loads the a texture from the given path
*/

bool TextureFromFile(const char * path,unsigned int & textureID)
{
	std::string filename = std::string(path);


	int width, height, comps;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &comps, 0);

	if (data)
	{
		glGenTextures(1, &textureID);

		GLenum format;
		switch (comps)
		{
		case 1:
		{
			format = GL_RED;
			break;
		}
		case 3:
		{
			format = GL_RGB;
			break;
		}
		case 4:
		{
			format = GL_RGBA;
			break;
		}
		default:
			std::cout << "Not valid format texture. " << std::endl;
			stbi_image_free(data);
			return false;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
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