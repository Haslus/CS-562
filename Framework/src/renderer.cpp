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



const int width = 1920;
const int height = 1080;

std::string m_win_name;
ImGuiWindowFlags m_flags;

/**
* @brief 	Function for error checking
* @param	error
* @param	error_message
*/
void error_callback(int error, const char* error_message)
{
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
	ImGui::ShowDemoWindow();
	renderImGUI();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
/**
* @brief 	Render ImGUI
*/
void Renderer::renderImGUI()
{
	ImGui::Begin("G-Buffer images", nullptr, m_flags);

	ImGui::Image((void*)(intptr_t)gPosition, ImVec2(256, 256), ImVec2(0, 1), (ImVec2(1, 0)));
	ImGui::Image((void*)(intptr_t)gNormal, ImVec2(256, 256), ImVec2(0, 1), (ImVec2(1, 0)));
	ImGui::Image((void*)(intptr_t)gAlbedoSpec, ImVec2(256, 256), ImVec2(0, 1), (ImVec2(1, 0)));
	ImGui::Image((void*)(intptr_t)gDepth, ImVec2(256, 256), ImVec2(0, 1), (ImVec2(1, 0)));
	ImGui::Image((void*)(intptr_t)lightTex, ImVec2(256, 256), ImVec2(0, 1), (ImVec2(1, 0)));
	ImGui::End();

	ImGui::Begin("Properties of the scene", nullptr, m_flags);


	if(ImGui::Button("Create Light in Origin"))
	{
		scene_lights.push_back(Light{ vec3(0,0,0),vec3(1,1,1),1,0.7f,1.8f,new Model(models[1]) });
	}

	if (ImGui::Button("Create Light in Random Position"))
	{
		const float min = 0;
		const float max = 50;
		float x = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
		float y = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
		float z = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
		scene_lights.push_back(Light{ vec3(x,y,z),vec3(1,1,1),1,0.007f,0.0011f, new Model(models[1]) });
	}
	
	if (!scene_lights.empty())
	{
		static int light_index = 0;
		ImGui::DragInt("Light Index", &light_index, 1, 0, scene_lights.size() - 1);
		ImGui::DragFloat3("Light Position", &scene_lights[light_index].model->transform.Position.x, 0.5f, -200, 200);
		ImGui::DragFloat3("Light Color", &scene_lights[light_index].color.x, 0.01f, 0, 1);

		/*float lightMax = std::fmaxf(std::fmaxf(scene_lights[light_index].color.r, scene_lights[light_index].color.g), scene_lights[light_index].color.b);
		scene_lights[light_index].radius = (-scene_lights[light_index].linear + glm::sqrt(scene_lights[light_index].linear * scene_lights[light_index].linear 
			- 4 * scene_lights[light_index].quadratic * (scene_lights[light_index].constant - (256.0 / 5.0) * lightMax)))
			/ (2 * scene_lights[light_index].quadratic);
			*/
		ImGui::InputFloat("Radius: ", &scene_lights[light_index].radius);
	}

	ImGui::DragFloat("Global Ambient", &ambient, 0.01f, 0.0f, 1.0f);

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
	window = glfwCreateWindow(width, height, "CS 562 Renderer", NULL, NULL);

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
	gBufferShader = Shader("./resources/shaders/deferred.vert", "./resources/shaders/deferred.frag");
	lightingPassShader = Shader("./resources/shaders/lighting_pass.vert", "./resources/shaders/lighting_pass.frag");
	shader = Shader("./resources/shaders/normal.vert", "./resources/shaders/normal.frag");
	renderShader = Shader("./resources/shaders/null.vert", "./resources/shaders/null.frag");

	proj = glm::perspective(glm::radians(90.f), (float)width / (float)height, 0.1f, 10000.f);

	
	glEnable(GL_CULL_FACE);

	//Create G-Buffer

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	//Position buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	//Normal buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
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
	//Depth buffer
	glGenTextures(1, &gDepth);
	glBindTexture(GL_TEXTURE_2D, gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);
	//Tell attachments
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//Create Light Buffer
	glGenFramebuffers(1, &lightBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, lightBuffer);

	glGenTextures(1, &lightTex);
	glBindTexture(GL_TEXTURE_2D, lightTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightTex, 0);

	unsigned int attachment = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &attachment);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	read_JSON("./data/scenes/scene.json");

	objects.push_back(Object(new Model(models[0])));

	//Configure lighting pass shader
	lightingPassShader.Use();
	lightingPassShader.SetInt("gPosition", 0);
	lightingPassShader.SetInt("gNormal", 1);
	lightingPassShader.SetInt("gAlbedoSpec", 2);


	//Create light
	scene_lights.push_back(Light{ vec3(0,0,0),vec3(1,1,1),1,0.7f,1.8f,new Model(models[1]) });
}

/**
* @brief 	Update the renderer
*/
void Renderer::render_update()
{
	
	while (!glfwWindowShouldClose(window))
	{
		
		float currentFrame = (double)glfwGetTime();
		dt = currentFrame - lastFrame;
		lastFrame = currentFrame;
		get_input();
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

		for (auto obj : objects)
			obj.Draw(gBufferShader);


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
		lightingPassShader.SetFloat("Ambient", ambient);
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
			lightingPassShader.SetFloat("light.Constant", scene_lights[i].constant);
			lightingPassShader.SetFloat("light.Linear", scene_lights[i].linear);
			lightingPassShader.SetFloat("light.Quadratic", scene_lights[i].quadratic);
			lightingPassShader.SetFloat("light.Radius", scene_lights[i].radius);

			scene_lights[i].model->Draw(lightingPassShader);
		}
		
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);

		//Render Quad with final texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glCullFace(GL_BACK);
		shader.Use();
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lightTex);
		renderQuad();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Render Lights
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		//glDepthMask(GL_TRUE);
		//glClear(GL_COLOR_BUFFER_BIT);
		renderShader.Use();
		renderShader.SetMat4("projection", proj);
		renderShader.SetMat4("view", m_cam.ViewMatrix);
		for (unsigned int i = 0; i < scene_lights.size(); i++)
		{

			/*scene_lights[i].model->transform.SetScale(glm::vec3(0.1f));
			scene_lights[i].model->Draw(renderShader);*/
			scene_lights[i].model->transform.SetScale(glm::vec3(1));
			scene_lights[i].model->Draw(renderShader);
		}

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
	gBufferShader = Shader("./resources/shaders/deferred.vert", "./resources/shaders/deferred.frag");
	lightingPassShader = Shader("./resources/shaders/lighting_pass.vert", "./resources/shaders/lighting_pass.frag");
	shader = Shader("./resources/shaders/normal.vert", "./resources/shaders/normal.frag");
	renderShader = Shader("./resources/shaders/null.vert", "./resources/shaders/null.frag");
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

int main_demo(int argc, char** argv)
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

/**
* @brief 	Renders a cube, used for Debugging
*/
void Renderer::renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);


}