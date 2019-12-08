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

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

bool TextureFromFile(const char* path, unsigned int & textureID);

GLuint loadDDS(const char * imagepath, int & _width, int & height);





struct Neighbors
{
	unsigned int n1;
	unsigned int n2;

	Neighbors()
	{
		n1 = n2 = (unsigned int)-1;
	}

	void AddNeigbor(unsigned int n)
	{
		if (n1 == -1) {
			n1 = n;
		}
		else if (n2 == -1) {
			n2 = n;
		}
		else {
			assert(0);
		}
	}

	unsigned int GetOther(unsigned int me) const
	{
		if (n1 == me) {
			return n2;
		}
		else if (n2 == me) {
			return n1;
		}
		else {
			assert(0);
		}

		return 0;
	}
};

class Edge
{
public:
	Edge(unsigned int _a, unsigned int _b)
	{
		assert(_a != _b);

		if (_a < _b)
		{
			a = _a;
			b = _b;
		}
		else
		{
			a = _b;
			b = _a;
		}
	}

	void Print()
	{
		printf("Edge %d %d\n", a, b);
	}

	unsigned int a;
	unsigned int b;
};

static unsigned int GetOppositeIndex(const aiFace& Face, const Edge& e)
{
	for (unsigned int i = 0; i < 3; i++) {
		unsigned int Index = Face.mIndices[i];

		if (Index != e.a && Index != e.b) {
			return Index;
		}
	}

	assert(0);

	return 0;
}

class CompareEdges
{
public:
	bool operator()(const Edge& Edge1, const Edge& Edge2) const
	{
		if (Edge1.a < Edge2.a) {
			return true;
		}
		else if (Edge1.a == Edge2.a) {
			return (Edge1.b < Edge2.b);
		}
		else {
			return false;
		}
	}
};

class CompareVectors
{
public:
	bool operator()(const glm::vec3& a, const glm::vec3& b) const
	{ 
		if (a.x < b.x) {
			return true;
		}
		else if (a.x == b.x) {
			if (a.y < b.y) {
				return true;
			}
			else if (a.y == b.y) {
				if (a.z < b.z) {
					return true;
				}
			}
		}

		return false;
	}

};
class Face
{
public:
	unsigned int Indices[3];

	unsigned int GetOppositeIndex(const Edge& e) const
	{
		for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(Indices); i++)
		{
			unsigned int Index = Indices[i];

			if (Index != e.a && Index != e.b)
				return Index;
		}

		assert(0);

		return 0;
	}
};

class lineAdjData
{
public:
	unsigned int vertex1;
	unsigned int vertex2;
	unsigned int opposite1;
	unsigned int opposite2;

	lineAdjData() {}
	lineAdjData(unsigned int v1, unsigned int v2,
		unsigned int o1, unsigned int o2) : vertex1(v1), vertex2(v2),
		opposite1(o1), opposite2(o2) {}

	bool operator==(const lineAdjData& other)
	{
		if (((vertex1 == other.vertex1 && vertex2 == other.vertex2) ||
			(vertex1 == other.vertex2 && vertex2 == other.vertex1)) &&
			((opposite1 == other.opposite1 && opposite2 == other.opposite2) ||
			(opposite1 == other.opposite2 && opposite2 == other.opposite1)))
		{
			return true;
		}

		return false;
	}
};

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
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> tri_indices, std::vector<unsigned int> line_indices,
		std::vector<Texture> textures);
	void Draw(Shader shader, bool wireframe, bool tessellation = false);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> triangle_indices;
	std::vector<unsigned int> lineadj_indices;
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

	Model(const std::string& path);
	void Draw(Shader shader, bool tessellation = false);

	Transform transform;

	unsigned ID = 0;

	bool wireframe = false;
private:

	std::vector<Mesh*> m_meshes;
	std::string m_directory;

	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
};

class Decal
{
public:
	Decal(const std::string & diffuse, const std::string & normal, Model * model);
	enum DrawMode
	{
		FULLDECAL, ONLYPIXELS, SHADING
	};
	void Draw(Shader shader, DrawMode drawMode);
private:
	Model * cube;
	std::vector<Texture> textures;
	
};

