
/* Start Header -------------------------------------------------------
Copyright (C) 2011 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen
Institute of Technology is prohibited.
File Name: Mesh.h
Purpose: Declaration of a Mesh class
Language: C++
Platform: Windows
Project: jon.sanchez_CS300_2
Author: Jon Sanchez
End Header --------------------------------------------------------*/
#pragma once

#include <vector>
#include <map>
#include <string>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include <GL/glew.h>
#include <GL/GL.h>



class MeshData
{	
public:	
	// Vertex struct used by the VBO indexer
	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;

		// Compares two vertices (used in the map search comparisons)
		bool operator<(const Vertex rhs) const
		{
			return memcmp((void*)this, (void*)&rhs, sizeof(Vertex)) > 0;
		};
	};


	
	// Constructor/Destructor
	MeshData();
	~MeshData();

	// Loads the OBJ file in the mesh
	bool LoadOBJ(const char * path);

	// Checks if the vertex has already been used
	bool IsVertexAlreadyUsed(	Vertex & v, 
								std::map<Vertex,unsigned short> & VertexToOutIndex,
								unsigned short & result);

	// Removes duplicated vertices and fills the data of the mesh correctly
	void IndexVBO(	std::vector<glm::vec3> & in_vertices, 
					std::vector<glm::vec2> & in_uvs,
					std::vector<glm::vec3> & in_normals);

	// Parses the face data of the OBJ files
	bool ParseFace(const std::string & line, int * vertexIndex, int * uvIndex, int * normalIndex);

	// Computes adjacency index data
	void AdjacencyIndexes();

	// Clear all data of mesh
	void ClearAll();

	// Bind mesh data
	void BindBuffers();
	
	// Bind mesh data vao
	void Use() const;
	
	// Get number of indices
	unsigned IndexCount() const;

	void CreateLine();
	void CreateQuad();

private:

	// Data
	std::vector<glm::vec3>			vertices_; 
	std::vector<glm::vec3>			normals_; 
	std::vector<glm::vec2>			uvs_;
	std::vector<unsigned short>		indexBuf_;

	// OpenGL
	GLuint VAO_;
	GLuint vertexbuffer_;
	GLuint uvbuffer_;
	GLuint normalbuffer_;
	GLuint indexbuffer_;
};

