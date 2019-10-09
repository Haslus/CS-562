/**
* @file		model.h
* @date 	02/08/2019
* @author	Asier Bilbao
* @par		Login: asier.b
* @par		Course: CS 562
* @par		Assignment #1
* @brief 	Framework
*
*
*/
#pragma once
#include "pch.h"
#include "transform.h"
#include "shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

bool TextureFromFile(const char* path, unsigned int & textureID);


struct Vertex
{

	glm::vec3 m_position;
	glm::vec3 m_normal;
	glm::vec2 m_texcoords;
	glm::vec3 m_tangent;
	glm::vec3 m_bitangent;

};

struct Texture {
	unsigned int m_id;
	std::string m_type;
	std::string m_path;
};



struct Material {
	
	Material() {};
	Material(aiColor4D specular, aiColor4D diffuse, aiColor4D ambient, float shininess)
	{
		this->specular.x = specular.r;
		this->specular.y = specular.g;
		this->specular.z = specular.b;
		this->specular.w = specular.a;

		this->diffuse.x = diffuse.r;
		this->diffuse.y = diffuse.g;
		this->diffuse.z = diffuse.b;
		this->diffuse.w = diffuse.a;

		this->ambient.x = ambient.r;
		this->ambient.y = ambient.g;
		this->ambient.z = ambient.b;
		this->ambient.w = ambient.a;

		this->shininess = shininess;
	}

	glm::vec4 specular;
	glm::vec4 diffuse;
	glm::vec4 ambient;
	float shininess;
};

class Mesh
{
public:
	Mesh() {};
	~Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, 
		std::vector<Texture> textures, Material material);
	void Draw(Shader shader);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Material material;
	std::vector<Texture> textures;
	std::string name;

private:
	void SetupMesh();

	unsigned int VAO, VBO, EBO;
	
};

class Model
{
public:

	Model(const std::string& path, bool gamma = false);
	void Draw(Shader shader);

	Transform transform;

	unsigned ID = 0;

	bool gammaCorrection;
private:

	std::vector<Mesh> m_meshes;
	std::string m_directory;

	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
};


