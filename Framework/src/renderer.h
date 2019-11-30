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
	std::vector<unsigned int> furTexture;
	std::vector<unsigned int> furTextureOffset;
	Shader furShader;
	int numShells = 1;
	unsigned int furTextureArray;
	unsigned int furTextureOffsetArray;
	//

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

	unsigned int gBuffer, lightBuffer, EDBuffer, blurBuffer, ambientBuffer,renderBuffer, bloomBuffer, blendBuffer, refinementDepthBuffer, decalBuffer, AOBuffer, BFBuffer[2];
	unsigned int pingpongBuffer[2];
	Shader gBufferShader;
	Shader lightingPassShader;
	Shader edgeDetectionShader;
	Shader blurShader;
	Shader ambientShader;
	Shader bloomShader;
	Shader gaussianblurShader;
	Shader blendShader;
	Shader tessellationShader;
	Shader decalShader;
	Shader HBAOShader;
	Shader BFShader;
	unsigned int gPosition, gNormal, gAlbedoSpec, gDepth, gLinearDepth;
	unsigned int lightTex;
	unsigned int EDTex;
	unsigned int blurTex;
	unsigned int ambientTex;
	unsigned int renderTex;
	unsigned int bloomTex;
	unsigned int pingpongTex[2];
	unsigned int finalpingpongTex;
	unsigned int blendTex;
	unsigned int AOTex;
	unsigned int randomTex;
	unsigned int BFAOTex[2];
	unsigned int pingpongTex_2[2];
	unsigned int finalpingpongTex_2;
	std::vector<Light> scene_lights;
	float ambient = 0;
	/*float tessLevels = 1.0;
	bool adaptiveTesellation = false;
	float LOD_distance = 1.0f;
	int LOD_pow = 3;
	bool LOD = false;
	float tessAlpha = 1.0f;*/
	glm::mat4x4 proj;
	std::vector<Object> objects;

	bool drawDecals = false;
	std::vector<Decal> decals;
	unsigned int renderTexture;
	int drawMode = 0;
	float angleLimit = 0.8f;

	//HBAO
	bool HBAOEnable = true;
	float radius = 10.f;
	float angleBias = 0.f;
	int numDirections = 4;
	int numSteps = 10;
	float attenuation = 1.7f;
	float scale = 1.f;

	float sigma_S = 6;
	float sigma_R = 0.5;
	bool bilateralFilter = true;
	int BF_size = 5;
	int GB_sIZE = 5;

	const float weight_3[3] = { 0.262014f, 0.225429f, 0.143564f };
	const float weight_5[5] = { 0.221569f, 0.190631f, 0.121403f, 0.057223f, 0.019959f };
	const float weight_7[7] = { 0.218884f, 0.188321f, 0.119932f, 0.05653f, 0.019717f, 0.005088f, 0.000971f };
	const float weight_9[9] = { 0.218818f, 0.188264f, 0.119895f, 0.056512f, 0.019711f, 0.005086f, 0.000971f, 0.000137f, 0.000014f };


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
