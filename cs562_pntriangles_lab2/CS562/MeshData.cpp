/* Start Header -------------------------------------------------------
Copyright (C) 2011 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen
Institute of Technology is prohibited.
File Name: Mesh.cpp
Purpose: Definition of a mesh
Language: C++
Platform: Windows
Project: jon.sanchez_CS300_2
Author: Jon Sanchez
End Header --------------------------------------------------------*/
#include "MeshData.h"

#include <GL/glew.h>
#include <GL/GL.h>

#include <fstream>
#include <unordered_map>


MeshData::MeshData()
{
	glGenVertexArrays(1, &VAO_);
	GLenum error = glGetError();

	glGenBuffers(1, &vertexbuffer_);
	glGenBuffers(1, &uvbuffer_);
	glGenBuffers(1, &normalbuffer_);
	glGenBuffers(1, &indexbuffer_);
	error = glGetError();
}


MeshData::~MeshData()
{
	glDeleteBuffers(1, &indexbuffer_);
	glDeleteBuffers(1, &normalbuffer_);
	glDeleteBuffers(1, &uvbuffer_);
	glDeleteBuffers(1, &vertexbuffer_);

	glDeleteVertexArrays(1, &VAO_);
}

void MeshData::ClearAll()
{
	vertices_.clear();
	normals_.clear();
	uvs_.clear();
	indexBuf_.clear();
}

bool MeshData::LoadOBJ(const char * path)
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	// OBJ data read
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;
	std::vector< glm::vec3 > temp_vertices;

	// Every combination of vertices to give to VBO indexer
	std::vector< glm::vec2 > in_uvs;
	std::vector< glm::vec3 > in_normals;
	std::vector< glm::vec3 > in_vertices;

	// Open OBJ file
	std::ifstream inFile(path);
	if(!inFile.is_open())
	{
		return false;
	}

	// Read until it breaks on EOF
	while(!inFile.eof())
	{
		std::string lineHeader;

		// Read the first word of the line
		
		inFile >> lineHeader;
		
		// Read lineHeader
		if (lineHeader == "v")
		{
			std::string value;
			// Add new vertex
			glm::vec3 vertex;

			inFile >> value;
			vertex.x = static_cast<float>(std::atof(value.c_str()));
			inFile >> value;
			vertex.y = static_cast<float>(std::atof(value.c_str()));
			inFile >> value;
			vertex.z = static_cast<float>(std::atof(value.c_str()));

			temp_vertices.push_back(vertex);
		}
		else if (lineHeader == "vt")
		{
			std::string value;

			// Add new uv coordinate
			glm::vec2 uv;

			inFile >> value;
			uv.x = static_cast<float>(std::atof(value.c_str()));
			inFile >> value;
			uv.y = static_cast<float>(std::atof(value.c_str()));

			temp_uvs.push_back(uv);
		}
		else if (lineHeader == "vn")
		{
			std::string value;
			// Add new normal
			glm::vec3 normal;

			inFile >> value;
			normal.x = static_cast<float>(std::atof(value.c_str()));
			inFile >> value;
			normal.y = static_cast<float>(std::atof(value.c_str()));
			inFile >> value;
			normal.z = static_cast<float>(std::atof(value.c_str()));

			temp_normals.push_back(normal);
		}
		else if (lineHeader == "f")
		{
			std::string faceLine;
			// Add new Face
			int vertexIndex[3], uvIndex[3], normalIndex[3];

			// Get whole face line
			std::getline(inFile, faceLine);

			if(!ParseFace(faceLine, vertexIndex, uvIndex,normalIndex))
			{
				ClearAll();
				return false;
			}

			// Decrement indexes 
			vertexIndex[0]--;
			vertexIndex[1]--;
			vertexIndex[2]--;
			normalIndex[0]--;
			normalIndex[1]--;
			normalIndex[2]--;
			uvIndex[0]--;
			uvIndex[1]--;
			uvIndex[2]--;

			// Create a new combination of position, normal, uv
			if(vertexIndex[0] >= 0)
			{
				in_vertices.push_back(temp_vertices[vertexIndex[0]]);
				in_vertices.push_back(temp_vertices[vertexIndex[1]]);
				in_vertices.push_back(temp_vertices[vertexIndex[2]]);
			}
			else
			{
				in_vertices.push_back(glm::vec3(0.0,0.0,0.0));
				in_vertices.push_back(glm::vec3(0.0,0.0,0.0));
				in_vertices.push_back(glm::vec3(0.0,0.0,0.0));
			}
			if(normalIndex[0] >= 0)
			{
				in_normals.push_back(temp_normals[normalIndex[0]]);
				in_normals.push_back(temp_normals[normalIndex[1]]);
				in_normals.push_back(temp_normals[normalIndex[2]]);
			}
			else
			{
				in_normals.push_back(glm::vec3(0.0,0.0,0.0));
				in_normals.push_back(glm::vec3(0.0,0.0,0.0));
				in_normals.push_back(glm::vec3(0.0,0.0,0.0));
			}
			if(uvIndex[0] >= 0)
			{
				in_uvs.push_back(temp_uvs[uvIndex[0]]);
				in_uvs.push_back(temp_uvs[uvIndex[1]]);
				in_uvs.push_back(temp_uvs[uvIndex[2]]);
			}
			else
			{
				in_uvs.push_back(glm::vec2(0.0,0.0));
				in_uvs.push_back(glm::vec2(0.0,0.0));
				in_uvs.push_back(glm::vec2(0.0,0.0));
			}
		}
		else
		{
			// Probably a comment
			std::string uselessBuffer;
			std::getline(inFile, uselessBuffer);
		}
	}

	// Creates the actual final buffers (vertices, uvs, normals) and index buffer checking the repeated
	IndexVBO(in_vertices, in_uvs, in_normals);

	return true;
}

bool MeshData::ParseFace(const std::string & line, int * vertexIndex, int * uvIndex, int * normalIndex)
{
	// Full data is given
	int matches = sscanf_s(line.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
	
	if (matches == 9)
		return true;

	return false;
}

void MeshData::AdjacencyIndexes()
{
	// Elements with adjacency info
	std::vector<unsigned short> adjacencyIndices(indexBuf_.size() * 2);

	// Copy actual triangles to new format 
	for (unsigned i = 0; i < indexBuf_.size(); i += 3)
	{
		adjacencyIndices[i * 2 + 0] = indexBuf_[i];
		adjacencyIndices[i * 2 + 1] = std::numeric_limits<unsigned short>::max();
		adjacencyIndices[i * 2 + 2] = indexBuf_[i + 1];
		adjacencyIndices[i * 2 + 3] = std::numeric_limits<unsigned short>::max();
		adjacencyIndices[i * 2 + 4] = indexBuf_[i + 2];
		adjacencyIndices[i * 2 + 5] = std::numeric_limits<unsigned short>::max();
	}

	struct Edge
	{
		Edge(unsigned short vertex0, unsigned short vertex1) : v0(vertex0), v1(vertex1) {}

		unsigned short v0;
		unsigned short v1;

		bool operator==(const Edge &other) const
		{
			return (v0 == other.v0 && v1 == other.v1)
				|| (v0 == other.v1 && v1 == other.v0);
		}
	};

	// Find matching edges
	for (unsigned i = 0; i < adjacencyIndices.size(); i += 6)
	{
		// A triangle
		unsigned short a1 = adjacencyIndices[i];
		unsigned short b1 = adjacencyIndices[i + 2];
		unsigned short c1 = adjacencyIndices[i + 4];

		Edge e1_1(a1, b1);
		Edge e1_2(b1, c1);
		Edge e1_3(c1, a1);
		
		// Find edges in the rest of the edges
		for (unsigned j = i + 6; j < adjacencyIndices.size(); j += 6)
		{
			unsigned short a2 = adjacencyIndices[j];
			unsigned short b2 = adjacencyIndices[j + 2];
			unsigned short c2 = adjacencyIndices[j + 4];

			Edge e2_1(a2, b2);
			Edge e2_2(b2, c2);
			Edge e2_3(c2, a2);

			// Edge 1 == Edge 1
			if (e1_1 == e2_1)
			{
				adjacencyIndices[i + 1] = c2;
				adjacencyIndices[j + 1] = c1;
			}
			// Edge 1 == Edge 2
			if (e1_1 == e2_2)
			{
				adjacencyIndices[i + 1] = a2;
				adjacencyIndices[j + 3] = c1;
			}
			// Edge 1 == Edge 3
			if (e1_1 == e2_3)
			{
				adjacencyIndices[i + 1] = b2;
				adjacencyIndices[j + 5] = c1;
			}
			// Edge 2 == Edge 1
			if (e1_2 == e2_1)
			{
				adjacencyIndices[i + 3] = c2;
				adjacencyIndices[j + 1] = a1;
			}
			// Edge 2 == Edge 2
			if (e1_2 == e2_2)
			{
				adjacencyIndices[i + 3] = a2;
				adjacencyIndices[j + 3] = a1;
			}
			// Edge 2 == Edge 3
			if (e1_2 == e2_3)
			{
				adjacencyIndices[i + 3] = b2;
				adjacencyIndices[j + 5] = a1;
			}
			// Edge 3 == Edge 1
			if (e1_3 == e2_1)
			{
				adjacencyIndices[i + 5] = c2;
				adjacencyIndices[j + 1] = b1;
			}
			// Edge 3 == Edge 2
			if (e1_3 == e2_2)
			{
				adjacencyIndices[i + 5] = a2;
				adjacencyIndices[j + 3] = b1;
			}
			// Edge 3 == Edge 3
			if (e1_3 == e2_3)
			{
				adjacencyIndices[i + 5] = b2;
				adjacencyIndices[j + 5] = b1;
			}
		}
	}

	// Look for edges that do not have adjacents
	for (GLuint i = 0; i < adjacencyIndices.size(); i += 6)
	{
		if (adjacencyIndices[i + 1] == std::numeric_limits<unsigned short>::max()) adjacencyIndices[i + 1] = adjacencyIndices[i + 4];
		if (adjacencyIndices[i + 3] == std::numeric_limits<unsigned short>::max()) adjacencyIndices[i + 3] = adjacencyIndices[i];
		if (adjacencyIndices[i + 5] == std::numeric_limits<unsigned short>::max()) adjacencyIndices[i + 5] = adjacencyIndices[i + 2];
	}

	// Change index buffer
	indexBuf_ = adjacencyIndices;
}

bool MeshData::IsVertexAlreadyUsed(	Vertex & v, 
								std::map<Vertex,unsigned short> & VertexToOutIndex,
								unsigned short & result)
{
	std::map<Vertex,unsigned short>::iterator it = VertexToOutIndex.find(v);
	// Search for the vertex
	if ( it == VertexToOutIndex.end() )
	{
		return false; // Does not exist, new vertex
	}
	else
	{
		// Vertex used, return the index
		result = it->second;
		return true;
	}
}



void MeshData::IndexVBO(	std::vector<glm::vec3> & in_vertices, 
							std::vector<glm::vec2> & in_uvs, 
							std::vector<glm::vec3> & in_normals)
{
	ClearAll();

	std::map<Vertex, unsigned short> VertexToOutIndex;

	// For each input vertex
	for (unsigned int i = 0; i < in_vertices.size(); i++)
	{

		Vertex v = {in_vertices[i], in_uvs[i], in_normals[i]};
		
		// Try to find if the vertex was used before
		unsigned short index;
		bool found = IsVertexAlreadyUsed(v, VertexToOutIndex, index);

		if (found)
		{ 
			// The vertex is already in the VBO, take its index
			indexBuf_.push_back( index );
		}
		else
		{ 
			// The vertex is new, add it to the output data
			vertices_.push_back( in_vertices[i]);
			uvs_.push_back( in_uvs[i]);
			normals_ .push_back( in_normals[i]);

			unsigned short newindex = (unsigned short)vertices_.size() - 1;
			indexBuf_ .push_back( newindex );
			VertexToOutIndex[v] = newindex;
		}
	}
}


void MeshData::BindBuffers()
{
	glBindVertexArray(VAO_);
	GLenum error = glGetError();

	// Bind a buffer of vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	error = glGetError();

	if (uvs_.size() > 0)
	{
		// Bind a buffer for the UV coordinates
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_);
		glBufferData(GL_ARRAY_BUFFER, uvs_.size() * sizeof(glm::vec2), &uvs_[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}
	

	error = glGetError();
	if (normals_.size() > 0)
	{
		// Bind a buffer for the indices
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_);
		glBufferData(GL_ARRAY_BUFFER, normals_.size() * sizeof(glm::vec3), &normals_[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}
			

	error = glGetError();
	// Bind a buffer for the indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuf_.size() * sizeof(unsigned short), &indexBuf_[0], GL_STATIC_DRAW);

	error = glGetError();
	glBindVertexArray(0);
}

// Bind mesh data vao
void MeshData::Use() const
{
	glBindVertexArray(VAO_);
}

unsigned MeshData::IndexCount() const
{
	return indexBuf_.size();
}

void MeshData::CreateLine()
{
	ClearAll();

	vertices_.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
	vertices_.push_back(glm::vec3(-0.8f, 1.0f, 0.0f));
	vertices_.push_back(glm::vec3(2.0f, -1.0f, 0.0f));
	vertices_.push_back(glm::vec3(1.0f, 1.0f, 0.0f));

	indexBuf_.push_back(0);
	indexBuf_.push_back(1);
	indexBuf_.push_back(2);
	indexBuf_.push_back(3);
}

void MeshData::CreateQuad()
{
	ClearAll();

	vertices_.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
	vertices_.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
	vertices_.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
	vertices_.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));

	indexBuf_.push_back(0);
	indexBuf_.push_back(1);
	indexBuf_.push_back(2);
	indexBuf_.push_back(3);
}