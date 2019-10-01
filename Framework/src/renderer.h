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
	void renderCube();
	void render_update();
	void render_exit();
	void get_input();

	void read_JSON(const std::string& path);

	Shader shader;
	unsigned ID = 1;

	float dt = 0.0f;
	float lastFrame = 0.0f;

	bool firstMouse = true;
	float lastX, lastY;

	Camera m_cam;
	vec2 m_mouse_position;

	std::vector<Model> models;

	unsigned int gBuffer;
	Shader gBufferShader;
	Shader lightingPassShader;
	unsigned int gPosition, gNormal, gAlbedoSpec;

	Light scene_light;
	std::vector<Light> scene_lights;
	float ambient = 0;

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
	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
};

int main_demo(int argc, char** argv);
