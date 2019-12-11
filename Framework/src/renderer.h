/**
* @file		renderer.h
* @date 	02/08/2019
* @author	Asier Bilbao
* @par		Login: asier.b
* @par		Course: CS 562
* @par		Assignment #1
* @brief 	Framework
*
*/
#pragma once
#include "object.h"
#include "model.h"
#include "shader.h"
#include "light.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


 

class Renderer
{
private:
	bool initImGUI();
	void updateImGUI();
	void renderImGUI();
	void exitImGUI();


	void render_initialize();
	void renderQuad();
	void render_update();
	void render_exit();
	void get_input();
	void updateShaders();
	void clear();
	void read_JSON(const std::string& path);
	void read_JSON_Scene(const std::string& path);

	//Furry
	void LoadFur();
	unsigned int colorTexture;
	std::vector<unsigned int> furTexture;
	std::vector<unsigned int> furTextureOffset;
	Shader furShader;
	unsigned int furTextureArray;
	unsigned int furTextureOffsetArray;
	bool showShells = true;

	bool showFins = true;
	Shader finShader;
	unsigned int finTexture;
	unsigned int finOffset;

	//Shader variables
	vec3 combVector = vec3(0,0,1);
	float combStrength = 0;
	int numShells = 1;
	float shellIncrement = 0.0001;
	float maxOpacity = 0.9f;

	//

	int currentModel = 0;
	int currentTexture = 0;

	std::vector<unsigned int> textureArray;

	Shader shader;
	Shader renderShader;
	unsigned ID = 1;

	float dt = 0.0f;
	float lastFrame = 0.0f;

	bool firstMouse = true;
	float lastX, lastY;

	Camera m_cam;
	vec2 m_mouse_position;

	std::vector<Model> models;


	glm::mat4x4 proj;
	std::vector<Object> objects;


public:
	Renderer() {};
	bool entry_point();
	GLFWwindow * window = nullptr;

	int prev_input1;
	int prev_input2;

	unsigned int quadVAO = 0;
	unsigned int quadVBO;
};

int main_demo();
