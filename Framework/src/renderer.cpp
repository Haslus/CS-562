/**
* @file		renderer.cpp
* @date 	02/08/2019
* @author	Asier Bilbao
* @par		Login: asier.b
* @par		Course: CS 562
* @par		Assignment #1
* @brief 	Framework
*
*/
#include "pch.h"
#include "renderer.h"
#include "opengl.h"
#include "model.h"
#include "json.hpp"

// for convenience
using json = nlohmann::json;


#include <random>
#include <ctime>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glm/gtc/matrix_transform.hpp>



int width;
int height;

std::string m_win_name;
ImGuiWindowFlags m_flags;

/**
* @brief 	Function for error checking
* @param	error
* @param	error_message
*/
void error_callback(int error, const char* error_message)
{
	std::cout << error;
	fprintf(stderr, "Error: %s\n", error_message);
}

/**
* @brief 	Renderer entry point
* @return	bool
*/
bool Renderer::entry_point()
{
	srand(static_cast <unsigned> (time(0)));
	render_initialize();
	render_update();
	render_exit();
	return true;
}
/**
* @brief 	Initialize ImGUI
*/
bool Renderer::initImGUI()
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
		return false;

	const char* imgui_version = "#version 130";

	if (!ImGui_ImplOpenGL3_Init(imgui_version))
		return false;

	m_win_name = "Editor";

	m_flags = 0;

	return true;
}
/**
* @brief 	Update ImGUI
*/
void Renderer::updateImGUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow();
	renderImGUI();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
/**
* @brief 	Render ImGUI
*/
void Renderer::renderImGUI()
{
	ImGui::Begin("All Buffers", nullptr, m_flags);

	ImGui::Text("Position G-Bufffer");
	if (ImGui::ImageButton((void*)(intptr_t)gPosition, ImVec2(480, 270), ImVec2(0, 1), (ImVec2(1, 0))))
		renderTexture = gPosition;
	ImGui::Text("Normal G-Bufffer");
	if(ImGui::ImageButton((void*)(intptr_t)gNormal,		ImVec2(480, 270), ImVec2(0, 1), (ImVec2(1, 0))))
		renderTexture = gNormal;
	ImGui::Text("Albedo G-Bufffer");
	if(ImGui::ImageButton((void*)(intptr_t)gAlbedoSpec,	ImVec2(480, 270), ImVec2(0, 1), (ImVec2(1, 0))))
		renderTexture = gAlbedoSpec;
	ImGui::Text("Linear Depth G-Bufffer");
	if(ImGui::ImageButton((void*)(intptr_t)gLinearDepth,	ImVec2(480, 270), ImVec2(0, 1), (ImVec2(1, 0))))
		renderTexture = gLinearDepth;
	ImGui::Text("Light Calculation");
	if(ImGui::ImageButton((void*)(intptr_t)lightTex,		ImVec2(480, 270), ImVec2(0, 1), (ImVec2(1, 0))))
		renderTexture = lightTex;
	ImGui::Text("Edge Detection");
	if(ImGui::ImageButton((void*)(intptr_t)EDTex,			ImVec2(480, 270), ImVec2(0, 1), (ImVec2(1, 0))))
		renderTexture = EDTex;
	ImGui::Text("Anti Aliasing (Not Light Model)");
	if(ImGui::ImageButton((void*)(intptr_t)blurTex,		ImVec2(480, 270), ImVec2(0, 1), (ImVec2(1, 0))))
		renderTexture = blurTex;
	ImGui::Text("Final Result No Post-Processing");
	if(ImGui::ImageButton((void*)(intptr_t)renderTex,		ImVec2(480, 270), ImVec2(0, 1), (ImVec2(1, 0))))
		renderTexture = renderTex;
	ImGui::Text("Bloom");
	if(ImGui::ImageButton((void*)(intptr_t)bloomTex,		ImVec2(480, 270), ImVec2(0, 1), (ImVec2(1, 0))))
		renderTexture = bloomTex;
	ImGui::Text("Blurred Bloom (Pingpong)");
	if(ImGui::ImageButton((void*)(intptr_t)finalpingpongTex, ImVec2(480, 270), ImVec2(0, 1), (ImVec2(1, 0))))
		renderTexture = finalpingpongTex;
	ImGui::Text("Final Result + Anti Aliasing + Bloom");
	if(ImGui::ImageButton((void*)(intptr_t)blendTex,		ImVec2(480, 270), ImVec2(0, 1), (ImVec2(1, 0))))
		renderTexture = blendTex;
	ImGui::End();

	ImGui::Begin("Properties of the scene", nullptr, m_flags);


	if(ImGui::Button("Create Light in Origin"))
	{
		scene_lights.push_back(Light{ vec3(0,0,0),vec3(1,1,1), new Model(models[1]) });
	}

	if (ImGui::Button("Create Light in Random Position"))
	{
		const float min_z = -50;
		const float max_z = 50;
		const float min_y = 25;
		const float max_y = 75;

		const float min_col = 0;
		const float max_col = 1;

		float y = min_y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_y - min_y)));
		float z = min_z + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_z - min_z)));
		float r = min_col + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_col - min_col)));
		float g = min_col + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_col - min_col)));
		float b = min_col + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_col - min_col)));

		scene_lights.push_back(Light{ vec3(0,y,z),vec3(r,g,b), new Model(models[1]) });
	}
	
	if (!scene_lights.empty())
	{
		static int light_index = 0;
		ImGui::DragInt("Light Index", &light_index, 1, 0, static_cast<int>(scene_lights.size() - 1));
		ImGui::DragFloat3("Light Position", &scene_lights[light_index].model->transform.Position.x, 0.5f, -200, 200);
		ImGui::DragFloat3("Light Color", &scene_lights[light_index].color.x, 0.1f, 0, 1);
		ImGui::InputFloat("Radius ", &scene_lights[light_index].radius);
		if(ImGui::Button("Pause Movement"))
		{
			for (auto & it : scene_lights)
				it.pause = true;
		}
		if (ImGui::Button("Resume Movement"))
		{
			for (auto & it : scene_lights)
				it.pause = false;
		}

	}

	ImGui::DragFloat("Global Ambient", &ambient, 0.01f, 0.0f, 1.0f);

	ImGui::Checkbox("Wireframe", &objects[0].model->wireframe);
	ImGui::Checkbox("Adaptive Tessellation", &adaptiveTesellation);
	ImGui::DragFloat("Tessellation Level", &tessLevels, 0.1f, 1, 100);
	
	ImGui::End();


}

/**
* @brief 	Exit ImGUI
*/
void Renderer::exitImGUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	clear();
}
/**
* @brief 	Initialize the renderer
*/
void Renderer::render_initialize()
{
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
		throw std::invalid_argument("GLFW could not be initialized");

	//Initialize Window
	window = glfwCreateWindow(1920, 1080, "CS 562 Renderer", NULL, NULL);
	glfwGetFramebufferSize(window, &width, &height);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

#if _DEBUG
	setup_gl_debug();
#endif

	//Initialize ImGUI
	initImGUI();

	//Load Shaders
	std::string tessellation[4] = { "./resources/shaders/deferred.vert", "./resources/shaders/tessellation.tcs",
		"./resources/shaders/tessellation.tes","./resources/shaders/deferred.frag" };

	gBufferShader = Shader(tessellation);
	lightingPassShader = Shader("./resources/shaders/lighting_pass.vert", "./resources/shaders/lighting_pass.frag");
	shader = Shader("./resources/shaders/normal.vert", "./resources/shaders/normal.frag");
	renderShader = Shader("./resources/shaders/null.vert", "./resources/shaders/null.frag");
	edgeDetectionShader = Shader("./resources/shaders/edge_detection.vert", "./resources/shaders/edge_detection.frag");
	blurShader = Shader("./resources/shaders/blur.vert", "./resources/shaders/blur.frag");
	ambientShader = Shader("./resources/shaders/ambient.vert", "./resources/shaders/ambient.frag");
	bloomShader = Shader("./resources/shaders/bloom.vert", "./resources/shaders/bloom.frag");
	gaussianblurShader = Shader("./resources/shaders/gaussianblur.vert", "./resources/shaders/gaussianblur.frag");
	blendShader = Shader("./resources/shaders/blend.vert", "./resources/shaders/blend.frag");

	
	//tessellationShader = Shader(tessellation);

	proj = glm::perspective(glm::radians(90.f), (float)width / (float)height, 0.1f, 1000.f);

	
	glEnable(GL_CULL_FACE);

	//Create G-Buffer

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	//Position buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	//Normal buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	//Albedo buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	//Ambient buffer
	glGenTextures(1, &ambientTex);
	glBindTexture(GL_TEXTURE_2D, ambientTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, ambientTex, 0);
	//Linear Depth buffer
	glGenTextures(1, &gLinearDepth);
	glBindTexture(GL_TEXTURE_2D, gLinearDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gLinearDepth, 0);
	//Tell attachments
	unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, attachments);

	//Depth buffer
	glGenTextures(1, &gDepth);
	glBindTexture(GL_TEXTURE_2D, gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//Create Light Buffer
	glGenFramebuffers(1, &lightBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, lightBuffer);

	glGenTextures(1, &lightTex);
	glBindTexture(GL_TEXTURE_2D, lightTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightTex, 0);

	unsigned int attachment = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &attachment);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Create Edge Detection Buffer
	glGenFramebuffers(1, &EDBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, EDBuffer);

	glGenTextures(1, &EDTex);
	glBindTexture(GL_TEXTURE_2D, EDTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, EDTex, 0);

	unsigned int attachment1 = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &attachment1);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Create Blurring Buffer
	glGenFramebuffers(1, &blurBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, blurBuffer);

	glGenTextures(1, &blurTex);
	glBindTexture(GL_TEXTURE_2D, blurTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTex, 0);

	unsigned int attachment2 = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &attachment2);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Create Render Buffer
	glGenFramebuffers(1, &renderBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderBuffer);

	glGenTextures(1, &renderTex);
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);

	unsigned int attachment4 = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &attachment4);

	//Depth buffer
	unsigned int grenderDepth;
	glGenTextures(1, &grenderDepth);
	glBindTexture(GL_TEXTURE_2D, grenderDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, grenderDepth, 0);

	//Create Bloom Buffer
	glGenFramebuffers(1, &bloomBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, bloomBuffer);

	glGenTextures(1, &bloomTex);
	glBindTexture(GL_TEXTURE_2D, bloomTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomTex, 0);

	unsigned int attachment3 = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &attachment3);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//Create Gaussian Blur Buffer
	glGenFramebuffers(2, pingpongBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, bloomBuffer);
	glGenTextures(2, pingpongTex);

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongBuffer[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongTex[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongTex[i], 0
		);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Create Blender Buffer
	glGenFramebuffers(1, &blendBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, blendBuffer);

	glGenTextures(1, &blendTex);
	glBindTexture(GL_TEXTURE_2D, blendTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blendTex, 0);

	unsigned int attachment5 = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &attachment5);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	read_JSON("./data/scenes/tessellation.json");

	objects.push_back(Object(new Model(models[0])));



	//Create light
	scene_lights.push_back(Light{ vec3(0,5,30),vec3(1,1,1),new Model(models[1]) });
	scene_lights.push_back(Light{ vec3(0,5,-30),vec3(1,1,1),new Model(models[1]) });
	renderTexture = blendTex;
}

/**
* @brief 	Update the renderer
*/
void Renderer::render_update()
{
	
	while (!glfwWindowShouldClose(window))
	{
		
		float currentFrame = static_cast<float>(glfwGetTime());
		dt = currentFrame - lastFrame;
		lastFrame = currentFrame;
		get_input();
		for (auto & it : scene_lights)
			it.update(dt);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Geometry Pass
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gBufferShader.Use();
		glDisable(GL_BLEND);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
		gBufferShader.SetMat4("projection", proj);
		gBufferShader.SetMat4("view", m_cam.ViewMatrix);
		gBufferShader.SetFloat("ambient", ambient);
		gBufferShader.SetFloat("uTessLevels", tessLevels);
		gBufferShader.SetBool("adaptiveTesellation", adaptiveTesellation);
		gBufferShader.SetVec3("camPos", vec3(glm::inverse(objects[0].model->transform.M2W) * vec4(m_cam.camPos,1)));
		//std::cout << m_cam.camPos.x << std::endl;
		for (auto & obj : objects)
			obj.Draw(gBufferShader, true);
		/////////////////////////////////////////

		//Lighting Pass
		glBindFramebuffer(GL_FRAMEBUFFER, lightBuffer);
		//Blending
		glDepthMask(GL_FALSE);
		
		glDepthFunc(GL_GREATER);
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glCullFace(GL_FRONT);
		glClear(GL_COLOR_BUFFER_BIT);
		lightingPassShader.Use();
		lightingPassShader.SetMat4("projection", proj);
		lightingPassShader.SetMat4("view", m_cam.ViewMatrix);
		lightingPassShader.SetVec3("viewPos", m_cam.camPos);
		lightingPassShader.SetVec2("ScreenSize", vec2(width, height));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

		
		for (unsigned int i = 0; i < scene_lights.size(); i++)
		{
			scene_lights[i].model->transform.SetScale(glm::vec3(scene_lights[i].radius));

			lightingPassShader.SetVec4("light.Position", vec4(scene_lights[i].model->transform.Position,1.f));
			lightingPassShader.SetVec4("light.Color", vec4(scene_lights[i].color,1));
			lightingPassShader.SetFloat("light.Radius", scene_lights[i].radius);

			scene_lights[i].model->Draw(lightingPassShader);
		}
		
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		/////////////////////////////////////////

		//Add Global Ambient
		glBindFramebuffer(GL_FRAMEBUFFER, renderBuffer);
		glClear(GL_COLOR_BUFFER_BIT);
		glCullFace(GL_BACK);
		glDisable(GL_DEPTH_TEST);
		ambientShader.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ambientTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, lightTex);
		renderQuad();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		////////////////////////////////////////

		/*Post-processing*/
		//Edge Detection
		glBindFramebuffer(GL_FRAMEBUFFER, EDBuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_BACK);
		glDisable(GL_DEPTH_TEST);
		edgeDetectionShader.Use();
		edgeDetectionShader.SetVec2("ScreenSize", vec2(width, height));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gLinearDepth);
		renderQuad();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		////////////////////////////////////////
		//Blur
		glBindFramebuffer(GL_FRAMEBUFFER, blurBuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_BACK);
		glDisable(GL_DEPTH_TEST);
		blurShader.Use();
		blurShader.SetVec2("ScreenSize", vec2(width, height));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, EDTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, renderTex);
		renderQuad();
		///////////////////////////////////////

		//Set Depth to that lights can be drawn
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderBuffer);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		//Render Lights
		glBindFramebuffer(GL_FRAMEBUFFER, renderBuffer);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		renderShader.Use();
		renderShader.SetMat4("projection", proj);
		renderShader.SetMat4("view", m_cam.ViewMatrix);
		for (unsigned int i = 0; i < scene_lights.size(); i++)
		{
			renderShader.SetVec3("color", scene_lights[i].color);
			scene_lights[i].model->transform.SetScale(glm::vec3(1));
			scene_lights[i].model->Draw(renderShader);
		}
		/////////////////////////////////////////

		//Bloom
		glBindFramebuffer(GL_FRAMEBUFFER, bloomBuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		bloomShader.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderTex);
		renderQuad();
		//////////////////////////////////////////

		//Gaussian Blur
		bool horizontal = true, first_it = true;
		int amount = 10;
		gaussianblurShader.Use();
		for (int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongBuffer[horizontal]);
			gaussianblurShader.SetInt("horizontal", horizontal);
			glBindTexture(GL_TEXTURE_2D, first_it ? bloomTex : pingpongTex[!horizontal]);
			renderQuad();
			horizontal = !horizontal;
			if (first_it)
				first_it = false;
		}
		finalpingpongTex = pingpongTex[!horizontal];
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/////////////////////////////////////////

		//Blend Gaussian Blur and Scene
		glBindFramebuffer(GL_FRAMEBUFFER, blendBuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		blendShader.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, blurTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, finalpingpongTex);
		renderQuad();
		////////////////////////////////////////


		//Render Quad with final texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		shader.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderTexture);
		renderQuad();
		/////////////////////////////////////////

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		updateImGUI();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
/**
* @brief 	Exit the renderer
*/
void Renderer::render_exit()
{
	exitImGUI();
	glfwDestroyWindow(window);
	glfwTerminate();
	clear();

}

/**
* @brief 	Get input from the user to move the camera
*/
void Renderer::get_input()
{

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (glfwGetKey(window, GLFW_KEY_F5)) {
		updateShaders();
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
	{

		float speed = 50.0f;
		auto  side = glm::normalize(glm::cross(m_cam.camFront, { 0, 1, 0 }));
		auto  up = glm::normalize(glm::cross(m_cam.camFront, side));

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
			speed *= 3.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_W)) {
			m_cam.camPos += glm::normalize(m_cam.camFront) * dt * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			m_cam.camPos -= glm::normalize(m_cam.camFront) * dt * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_A)) {
			m_cam.camPos -= glm::normalize(side) * dt * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			m_cam.camPos += glm::normalize(side) * dt * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			m_cam.camPos -= up * dt * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
			m_cam.camPos += up * dt * speed;
		}


		// View
		vec2 cursor_delta = { (float)xpos - m_mouse_position.x, (float)ypos - m_mouse_position.y };
		m_cam.camFront = vec3(vec4(m_cam.camFront, 0) * glm::rotate(glm::radians(15.0f) * dt * 0.5f * cursor_delta.y, side));
		m_cam.camFront = vec3(vec4(m_cam.camFront, 0) * glm::rotate(glm::radians(15.0f) * dt * 0.5f *cursor_delta.x, vec3(0, 1, 0)));

		m_cam.RecalculateViewMatrix();

	}

	m_mouse_position = { xpos,ypos };

	m_cam.RecalculateViewMatrix();

}
void Renderer::updateShaders()
{
	//Load Shaders
	std::string tessellation[4] = { "./resources/shaders/deferred.vert", "./resources/shaders/tessellation.tcs",
		"./resources/shaders/tessellation.tes","./resources/shaders/deferred.frag" };

	gBufferShader = Shader(tessellation);
	lightingPassShader = Shader("./resources/shaders/lighting_pass.vert", "./resources/shaders/lighting_pass.frag");
	shader = Shader("./resources/shaders/normal.vert", "./resources/shaders/normal.frag");
	renderShader = Shader("./resources/shaders/null.vert", "./resources/shaders/null.frag");
	edgeDetectionShader = Shader("./resources/shaders/edge_detection.vert", "./resources/shaders/edge_detection.frag");
	blurShader = Shader("./resources/shaders/blur.vert", "./resources/shaders/blur.frag");
	ambientShader = Shader("./resources/shaders/ambient.vert", "./resources/shaders/ambient.frag");
	bloomShader = Shader("./resources/shaders/bloom.vert", "./resources/shaders/bloom.frag");
	gaussianblurShader = Shader("./resources/shaders/gaussianblur.vert", "./resources/shaders/gaussianblur.frag");
	blendShader = Shader("./resources/shaders/blend.vert", "./resources/shaders/blend.frag");
}
void Renderer::clear()
{
	unsigned int gTextures[5] = { gPosition,gNormal ,gAlbedoSpec ,gDepth,gLinearDepth };

	glDeleteFramebuffers(1, &gBuffer);
	glDeleteTextures(5, gTextures);

	glDeleteFramebuffers(1, &lightBuffer);
	glDeleteTextures(1, &lightTex);

	glDeleteFramebuffers(1, &EDBuffer);
	glDeleteTextures(1, &EDTex);

	glDeleteFramebuffers(1, &blurBuffer);
	glDeleteTextures(1, &blurTex);

	glDeleteFramebuffers(1, &ambientBuffer);
	glDeleteTextures(1, &ambientTex);

	glDeleteFramebuffers(1, &renderBuffer);
	glDeleteTextures(1, &renderTex);

	glDeleteFramebuffers(1, &bloomBuffer);
	glDeleteTextures(1, &bloomTex);

	glDeleteFramebuffers(1, &blendBuffer);
	glDeleteTextures(2, pingpongTex);


}
/**
* @brief 	Read the JSON file and build the scene
*/
void Renderer::read_JSON(const std::string & path)
{
	std::ifstream i(path);
	json j = json::parse(i);

	json obj = j["objects"];

	for (auto it : obj)
	{
		std::string mesh = it["mesh"];

		json t = it["translation"];
		float t_x = t["x"];
		float t_y = t["y"];
		float t_z = t["z"];

		json r = it["rotate"];
		float r_x = r["x"];
		float r_y = r["y"];
		float r_z = r["z"];

		json s = it["scale"];
		float s_x = s["x"];
		float s_y = s["y"];
		float s_z = s["z"];

		Model model(mesh);
		model.transform.SetPosition({ t_x,t_y,t_z });
		model.transform.SetRotation({ r_x,r_y,r_z });
		model.transform.SetScale({ s_x,s_y,s_z });

		models.push_back(model);
	}


}

int main_demo()
{
	static Renderer render;
	render.entry_point();
	return 1;
};

/**
* @brief 	Renders a Quad, used to draw the buffer
*/
void Renderer::renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}