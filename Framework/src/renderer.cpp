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
#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII


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
#include <glm/gtc/random.hpp>

#include "glsc2ext.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int width;
int height;

std::string m_win_name;
ImGuiWindowFlags m_flags;

vec2 offset;


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	(void*)window;
	offset = { xoffset,yoffset };
}

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
	/*ImGui::Begin("All Buffers", nullptr, m_flags);

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
	ImGui::Text("Anti Aliasing");
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
	ImGui::Text("Random Texture");
	if (ImGui::ImageButton((void*)(intptr_t)randomTex, ImVec2(480, 270), ImVec2(0, 1), (ImVec2(1, 0))))
		renderTexture = randomTex;
	ImGui::Text("Ambient Occlusion");
	if (ImGui::ImageButton((void*)(intptr_t)AOTex,			ImVec2(480, 270), ImVec2(0, 1), (ImVec2(1, 0))))
		renderTexture = AOTex;
	ImGui::Text("Ambient Occlusion with Filtering");
	if (ImGui::ImageButton((void*)(intptr_t)finalpingpongTex_2,			 ImVec2(480, 270), ImVec2(0, 1), (ImVec2(1, 0))))
		renderTexture = finalpingpongTex_2;
	ImGui::Text("Final Result + Anti Aliasing + Bloom + Ambient Occlusion (If Enabled)");
	if (ImGui::ImageButton((void*)(intptr_t)blendTex, ImVec2(480, 270), ImVec2(0, 1), (ImVec2(1, 0))))
		renderTexture = blendTex;
	ImGui::End();*/

	ImGui::Begin("Fur and Shells", nullptr, m_flags);

	ImGui::InputInt("Shells", &numShells);

	ImGui::Checkbox("Orbital", &m_cam.orbital);
	ImGui::Checkbox("Show Fins", &showFins);


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

	glfwSetScrollCallback(window, scroll_callback);

#if _DEBUG
	setup_gl_debug();
#endif

	//Initialize ImGUI
	initImGUI();

	//Load Shaders
	//std::string tessellation[4] = { "./resources/shaders/deferred.vert", "./resources/shaders/tessellation.tcs",
	//	"./resources/shaders/tessellation.tes","./resources/shaders/deferred.frag" };

	//gBufferShader = Shader("./resources/shaders/deferred.vert","./resources/shaders/deferred.frag" );
	//lightingPassShader = Shader("./resources/shaders/lighting_pass.vert", "./resources/shaders/lighting_pass.frag");
	//renderShader = Shader("./resources/shaders/null.vert", "./resources/shaders/null.frag");
	//edgeDetectionShader = Shader("./resources/shaders/edge_detection.vert", "./resources/shaders/edge_detection.frag");
	//blurShader = Shader("./resources/shaders/blur.vert", "./resources/shaders/blur.frag");
	//ambientShader = Shader("./resources/shaders/ambient.vert", "./resources/shaders/ambient.frag");
	//bloomShader = Shader("./resources/shaders/bloom.vert", "./resources/shaders/bloom.frag");
	//gaussianblurShader = Shader("./resources/shaders/gaussianblur.vert", "./resources/shaders/gaussianblur.frag");
	//blendShader = Shader("./resources/shaders/blend.vert", "./resources/shaders/blend.frag");
	//decalShader = Shader("./resources/shaders/decals.vert", "./resources/shaders/decals.frag");
	//HBAOShader = Shader("./resources/shaders/hbao.vert", "./resources/shaders/hbao.frag");
	//BFShader = Shader("./resources/shaders/bilateralfilter.vert", "./resources/shaders/bilateralfilter.frag");
	
	std::string fur[3] = { "./resources/shaders/Fur/fins.vert", "./resources/shaders/Fur/fins.geo",
		"./resources/shaders/Fur/fins.frag"};

	shader = Shader("./resources/shaders/normal.vert", "./resources/shaders/normal.frag");
	furShader = Shader("./resources/shaders/Fur/fur.vert", "./resources/shaders/Fur/fur.frag");
	finShader = Shader(fur);
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

	//Refinment depth buffer
	//glGenTextures(1, &refinementDepthBuffer);
	//glBindTexture(GL_TEXTURE_2D, refinementDepthBuffer);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, refinementDepthBuffer, 0);

	//Tell attachments
	unsigned int attachments[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4 };
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

	//Depth buffer
	unsigned int blurDepth;
	glGenTextures(1, &blurDepth);
	glBindTexture(GL_TEXTURE_2D, blurDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, blurDepth, 0);

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

	//Create Decal Buffer
	glGenFramebuffers(1, &decalBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, decalBuffer);

	//Normal buffer
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gNormal, 0);
	//Albedo buffer
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gAlbedoSpec, 0);

	unsigned int attachment6[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachment6);

	glBindTexture(GL_TEXTURE_2D, gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Create Ambient Occlusion Buffer
	glGenFramebuffers(1, &AOBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, AOBuffer);

	//Albedo buffer
	glGenTextures(1, &AOTex);
	glBindTexture(GL_TEXTURE_2D, AOTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, AOTex, 0);

	unsigned int attachment7 = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, &attachment7);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Create Bilateral Filtering Buffer
	glGenFramebuffers(2, BFBuffer);
	glGenTextures(2, BFAOTex);

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, BFBuffer[i]);
		glBindTexture(GL_TEXTURE_2D, BFAOTex[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BFAOTex[i], 0
		);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	

	//Generate random texture
	glGenTextures(1, &randomTex);
	glBindTexture(GL_TEXTURE_2D, randomTex);

	std::vector<glm::vec3> colorData;
	for (int i = 0; i < width * height; i++)
	{
		glm::vec3 color;
		color.r = glm::linearRand(0.0f, 1.0f);
		color.g = glm::linearRand(0.0f, 1.0f);
		color.b = glm::linearRand(0.0f, 1.0f);
		colorData.push_back(color);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, colorData.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	
	read_JSON("./data/scenes/scene.json");


	//objects.push_back(Object(new Model(models[0])));



	//Create light
	scene_lights.push_back(Light{ vec3(0,15,0),vec3(1,1,1),new Model(models[0]),150 });

	scene_lights.push_back(Light{ vec3(-90,20,40),vec3(1,1,1),new Model(models[0]),150 });

	scene_lights.push_back(Light{ vec3(40,30,-40),vec3(1,1,1),new Model(models[0]),150 });

	scene_lights.push_back(Light{ vec3(75,30,0),vec3(1,1,1),new Model(models[0]),150 });
	//scene_lights.push_back(Light{ vec3(0,5,-30),vec3(1,1,1),new Model(models[1]) });
	renderTexture = blendTex;

	//Furry Stuff
	LoadFur();
	//
}

/**
* @brief 	Update the renderer
*/
void Renderer::render_update()
{
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(-1);
	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = static_cast<float>(glfwGetTime());
		dt = currentFrame - lastFrame;
		lastFrame = currentFrame;
		get_input();

		glClearColor(0.20f, 0.20f, 0.20f, 1.0f);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		//Render Mesh
		
		shader.Use();
		shader.SetMat4("proj", proj);
		shader.SetMat4("view", m_cam.ViewMatrix);
		models[0].Draw(shader,false);

		//Render Fins
		if (showFins)
		{
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_CULL_FACE);
			//glCullFace(GL_FRONT);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glDepthFunc(GL_LESS);

			finShader.Use();
			finShader.SetMat4("proj", proj);
			finShader.SetMat4("view", m_cam.ViewMatrix);
			finShader.SetVec3("Eye", vec3(glm::inverse(models[0].transform.M2W) * vec4(m_cam.camPos, 1)));
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, finTexture);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, finOffset);

			models[0].Draw(finShader, true);

		}

		
		//Render Shells
		//glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
		//glBlendEquation(GL_FUNC_ADD);
		//glBlendFunc(GL_ONE, GL_ONE);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		furShader.Use();
		furShader.SetMat4("proj", proj);
		furShader.SetMat4("view", m_cam.ViewMatrix);
		furShader.SetVec3("Eye", vec3(glm::inverse(models[0].transform.M2W) * vec4(m_cam.camPos,1)));
		furShader.SetVec3("Light", vec3(glm::inverse(models[0].transform.M2W) *  vec4((-100.0f, 300.0f, -200.0f,1))));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D_ARRAY, furTextureArray);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D_ARRAY, furTextureOffsetArray);

		furShader.SetVec3("combVector", vec3(0,0,1));
		furShader.SetFloat("combStrength", 0.3f);
		furShader.SetInt("numShells",15);
		furShader.SetFloat("shellIncrement",0.2f);

		for (int i = 1; i <= numShells; i++)
		{	
			furShader.SetInt("shell", i);
			models[0].Draw(furShader);
		}
		

	
		updateImGUI();
		glfwSwapBuffers(window);
		glfwPollEvents();

		continue;

#pragma region Stuff
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
		//std::cout << m_cam.camPos.x << std::endl;
		for (auto & obj : objects)
			obj.Draw(gBufferShader, false);
		/////////////////////////////////////////
		if (drawDecals)
		{
			//Decal Pass
			glBindFramebuffer(GL_FRAMEBUFFER, decalBuffer);
			//glCullFace(GL_BACK);
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			decalShader.Use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gDepth);
			decalShader.SetMat4("projection", proj);
			decalShader.SetMat4("view", m_cam.ViewMatrix);
			decalShader.SetVec2("screenSize", vec2(width, height));
			decalShader.SetFloat("angleLimit", angleLimit);
			for (auto & dec : decals)
				dec.Draw(decalShader, static_cast<Decal::DrawMode>(drawMode));
		}

		/////////////////////////////////////////


		//Horizon Based Ambient Occlusion


		if (HBAOEnable)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, AOBuffer);
			glClear(GL_COLOR_BUFFER_BIT);

			glCullFace(GL_BACK);
			glDisable(GL_DEPTH_TEST);

			HBAOShader.Use();
			HBAOShader.SetMat4("projection", proj);
			HBAOShader.SetMat4("view", m_cam.ViewMatrix);
			HBAOShader.SetVec2("ScreenSize", vec2(width, height));

			HBAOShader.SetFloat("radius", radius);
			HBAOShader.SetFloat("angleBias", angleBias);
			HBAOShader.SetInt("numDirections", numDirections);
			HBAOShader.SetInt("numSteps", numSteps);
			HBAOShader.SetFloat("attenuation", attenuation);
			HBAOShader.SetFloat("scale", scale);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gNormal);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gPosition);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, randomTex);
			renderQuad();


			//Bilateral Filtering
			if (bilateralFilter)
			{
				bool horizontal = true, first_it = true;
				int amount = 10;
				BFShader.Use();
				BFShader.SetFloat("sigma_S", sigma_S);
				BFShader.SetFloat("sigma_R", sigma_R);
				BFShader.SetInt("size", BF_size);
				for (int i = 0; i < amount; i++)
				{
					glBindFramebuffer(GL_FRAMEBUFFER, BFBuffer[horizontal]);
					BFShader.SetInt("horizontal", horizontal);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, first_it ? AOTex : BFAOTex[!horizontal]);
					renderQuad();
					horizontal = !horizontal;
					if (first_it)
						first_it = false;
				}
				finalpingpongTex_2 = BFAOTex[horizontal];
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			else
			{
				bool horizontal = true, first_it = true;
				int amount = 10;
				gaussianblurShader.Use();
				gaussianblurShader.SetInt("size", GB_sIZE);

				for (int i = 0; i < GB_sIZE; i++)
				{
					if (GB_sIZE == 5)
					{
						gaussianblurShader.SetFloat("weight[" + std::to_string(i) + ']', weight_5[i]);
					}
					else if (GB_sIZE == 7)
					{
						gaussianblurShader.SetFloat("weight[" + std::to_string(i) + ']', weight_7[i]);
					}
					else if (GB_sIZE == 3)
					{
						gaussianblurShader.SetFloat("weight[" + std::to_string(i) + ']', weight_3[i]);
					}
				}

				for (int i = 0; i < amount; i++)
				{
					glBindFramebuffer(GL_FRAMEBUFFER, BFBuffer[horizontal]);
					gaussianblurShader.SetInt("horizontal", horizontal);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, first_it ? AOTex : BFAOTex[!horizontal]);
					renderQuad();
					horizontal = !horizontal;
					if (first_it)
						first_it = false;
				}
				finalpingpongTex_2 = BFAOTex[horizontal];
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
		}

		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, AOBuffer);
			glClear(GL_COLOR_BUFFER_BIT);
			glBindFramebuffer(GL_FRAMEBUFFER, BFBuffer[0]);
			glClear(GL_COLOR_BUFFER_BIT);
			glBindFramebuffer(GL_FRAMEBUFFER, BFBuffer[1]);
			glClear(GL_COLOR_BUFFER_BIT);
		}



		////////////////////////////////////////


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
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, finalpingpongTex_2);
		lightingPassShader.SetBool("HBAO", HBAOEnable);

		for (unsigned int i = 0; i < scene_lights.size(); i++)
		{
			scene_lights[i].model->transform.SetScale(glm::vec3(scene_lights[i].radius));

			lightingPassShader.SetVec4("light.Position", vec4(scene_lights[i].model->transform.Position, 1.f));
			lightingPassShader.SetVec4("light.Color", vec4(scene_lights[i].color, 1));
			lightingPassShader.SetFloat("light.Radius", scene_lights[i].radius);

			scene_lights[i].model->Draw(lightingPassShader);
		}

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		/////////////////////////////////////////

		////Add Global Ambient
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

		//Set Depth so that lights can be drawn
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

		//Also add lights to the blurTex
		//Render Lights
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, blurBuffer);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, blurBuffer);
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
		{
			bool horizontal = true, first_it = true;
			int amount = 5;
			gaussianblurShader.Use();
			for (int i = 0; i < amount; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongBuffer[horizontal]);
				gaussianblurShader.SetInt("horizontal", horizontal);
				gaussianblurShader.SetInt("size", 5);
				for (int i = 0; i < GB_sIZE; i++)
				{

					gaussianblurShader.SetFloat("weight[" + std::to_string(i) + ']', weight_5[i]);

				}
				glBindTexture(GL_TEXTURE_2D, first_it ? bloomTex : pingpongTex[!horizontal]);
				renderQuad();
				horizontal = !horizontal;
				if (first_it)
					first_it = false;
			}
			finalpingpongTex = pingpongTex[!horizontal];
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
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
#pragma endregion

		

		
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
		if (m_cam.orbital)
		{

			if (glfwGetKey(window, GLFW_KEY_Q)) {

				m_cam.Radius++;

			}
			if (glfwGetKey(window, GLFW_KEY_E)) {

				m_cam.Radius--;

			}

			m_cam.Radius -= offset.y;
			offset.y = 0;
			// View
			vec2 cursor_delta = { (float)xpos - m_mouse_position.x, (float)ypos - m_mouse_position.y };
			const float angleSpeed = 0.001f;
			m_cam.AngleX += cursor_delta.x * angleSpeed;
			const float max = glm::pi<float>() / 2.f;
			float delta = cursor_delta.y * angleSpeed;
			if (((m_cam.AngleY + delta) < max) && (m_cam.AngleY + delta) > (-max))
			{
				m_cam.AngleY += delta;
			}


		}

		else
		{
			float speed = 50.0f;


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
				m_cam.camPos -= glm::normalize(m_cam.camRight) * dt * speed;
			}
			if (glfwGetKey(window, GLFW_KEY_D)) {
				m_cam.camPos += glm::normalize(m_cam.camRight) * dt * speed;
			}
			if (glfwGetKey(window, GLFW_KEY_SPACE)) {
				m_cam.camPos += m_cam.camUp * dt * speed;
			}
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
				m_cam.camPos -= m_cam.camUp * dt * speed;
			}

			vec2 cursor_delta = { (float)xpos - m_mouse_position.x, (float)ypos - m_mouse_position.y };
			const float angleSpeed = 0.1f;
			m_cam.axisAngle += vec3(cursor_delta.y * angleSpeed, cursor_delta.x * angleSpeed, 0);

			glm::quat q = glm::quat(glm::radians(m_cam.axisAngle));

			m_cam.quaternion = q;


			m_cam.RecalculateViewMatrix();
		}

		

	}

	m_mouse_position = { xpos,ypos };

	m_cam.RecalculateViewMatrix();

}
void Renderer::updateShaders()
{
	std::string fur[3] = { "./resources/shaders/Fur/fins.vert", "./resources/shaders/Fur/fins.geo",
		"./resources/shaders/Fur/fins.frag" };

	shader = Shader("./resources/shaders/normal.vert", "./resources/shaders/normal.frag");
	furShader = Shader("./resources/shaders/Fur/fur.vert", "./resources/shaders/Fur/fur.frag");
	finShader = Shader(fur);

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
* @brief 	Read the myJSON file and build the scene
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
/**
* @brief 	Read the JSON file and build the scene
*/
void Renderer::read_JSON_Scene(const std::string & path)
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

		Model * model = new Model(mesh);
		model->transform.SetPosition({ t_x,t_y,t_z });
		model->transform.SetRotation({ r_x,r_y,r_z });
		model->transform.SetScale({ s_x,s_y,s_z });

		objects.push_back(Object(model));
	}

	json dec = j["decals"];
	for (auto it : dec)
	{
		std::string diffuseTexture = it["diffuse"];
		std::string normalTexture = it["normal"];

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

		Model * cube = new Model(models[1]);
		cube->transform.SetPosition({ t_x,t_y,t_z });
		cube->transform.SetRotation({ r_x,r_y,r_z });
		cube->transform.SetScale({ s_x,s_y,s_z });

		Decal decal(diffuseTexture,normalTexture, cube);

		decals.push_back(decal);
	}

}

void Renderer::LoadFur()
{
	const std::string furTextures[] = {
		"./data/FurTexture/PNG/FurTexture00.png",
		"./data/FurTexture/PNG/FurTexture01.png",
		"./data/FurTexture/PNG/FurTexture02.png",
		"./data/FurTexture/PNG/FurTexture03.png",
		"./data/FurTexture/PNG/FurTexture04.png",
		"./data/FurTexture/PNG/FurTexture05.png",
		"./data/FurTexture/PNG/FurTexture06.png",
		"./data/FurTexture/PNG/FurTexture07.png",
		"./data/FurTexture/PNG/FurTexture08.png",
		"./data/FurTexture/PNG/FurTexture09.png",
		"./data/FurTexture/PNG/FurTexture10.png",
		"./data/FurTexture/PNG/FurTexture11.png",
		"./data/FurTexture/PNG/FurTexture12.png",
		"./data/FurTexture/PNG/FurTexture13.png",
		"./data/FurTexture/PNG/FurTexture14.png",
		"./data/FurTexture/PNG/FurTexture15.png" };

	glGenTextures(1, &furTextureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, furTextureArray);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY,
		1,                    //5 mipmaps
		GL_RGBA8,               //Internal format
		128, 128,           //width,height
		256                   //Number of layers
	);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	for (int i = 0; i < 16; i++)
	{
		int width, height, comps;
		unsigned char* data = stbi_load(furTextures[i].c_str(), &width, &height, &comps, 0);
		
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
			return;
		}

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
			0,                      //Mipmap number
			0, 0, i, //xoffset, yoffset, zoffset
			width, height, 1,          //width, height, depth
			format,                 //format
			GL_UNSIGNED_BYTE,       //type
			data); //pointer to data

	}

	const std::string furOffset[] = {
		"./data/FurTexture/PNG/FurTextureOffset00.png",
		"./data/FurTexture/PNG/FurTextureOffset01.png",
		"./data/FurTexture/PNG/FurTextureOffset02.png",
		"./data/FurTexture/PNG/FurTextureOffset03.png",
		"./data/FurTexture/PNG/FurTextureOffset04.png",
		"./data/FurTexture/PNG/FurTextureOffset05.png",
		"./data/FurTexture/PNG/FurTextureOffset06.png",
		"./data/FurTexture/PNG/FurTextureOffset07.png",
		"./data/FurTexture/PNG/FurTextureOffset08.png",
		"./data/FurTexture/PNG/FurTextureOffset09.png",
		"./data/FurTexture/PNG/FurTextureOffset10.png",
		"./data/FurTexture/PNG/FurTextureOffset11.png",
		"./data/FurTexture/PNG/FurTextureOffset12.png",
		"./data/FurTexture/PNG/FurTextureOffset13.png",
		"./data/FurTexture/PNG/FurTextureOffset14.png",
		"./data/FurTexture/PNG/FurTextureOffset15.png" };


	glGenTextures(1, &furTextureOffsetArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, furTextureOffsetArray);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY,
		1,                    //5 mipmaps
		GL_RGBA8,               //Internal format
		128, 128,           //width,height
		256                   //Number of layers
	);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);


	for (int i = 0; i < 16; i++)
	{
		int width, height, comps;
		unsigned char* data = stbi_load(furOffset[i].c_str(), &width, &height, &comps, 0);

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
			return;
		}

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
			0,                      //Mipmap number
			0, 0, i, //xoffset, yoffset, zoffset
			width, height, 1,          //width, height, depth
			format,                 //format
			GL_UNSIGNED_BYTE,       //type
			data); //pointer to data
	}

	TextureFromFile("./data/FurTexture/PNG/FurTextureFin.png", finTexture);
	TextureFromFile("./data/FurTexture/PNG/FurTextureOffsetFin.png", finOffset);


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