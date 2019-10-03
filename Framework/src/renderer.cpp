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
	ImGui::Image((void*)(intptr_t)lightTex, ImVec2(256, 256), ImVec2(0, 1), (ImVec2(1, 0)));
	ImGui::End();

	ImGui::Begin("Properties of the scene", nullptr, m_flags);


	if(ImGui::Button("Create Light in Origin"))
	{
		scene_lights.push_back(Light{ vec3(0,25,0),vec3(1,1,1),1,0.007f,0.0011f, new Model(models[1]) });
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
		ImGui::DragFloat3("Light Position", &scene_lights[light_index].position.x, 0.5f, -200, 200);
		ImGui::DragFloat3("Light Color", &scene_lights[light_index].color.x, 0.01f, 0, 1);
		ImGui::DragFloat("Light Constant", &scene_lights[light_index].constant, 0.001f, 0, 1,"%.4f");
		ImGui::DragFloat("Light Linear", &scene_lights[light_index].linear, 0.001f, 0, 1, "%.6f");
		ImGui::DragFloat("Light Quadratic", &scene_lights[light_index].quadratic, 0.00001f, 0, 1, "%.8f");
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

	//Initialize ImGUI
	initImGUI();

	//Load Shaders
	gBufferShader = Shader("./resources/shaders/deferred.vert", "./resources/shaders/deferred.frag");
	lightingPassShader = Shader("./resources/shaders/lighting_pass.vert", "./resources/shaders/lighting_pass.frag");
	shader = Shader("./resources/shaders/normal.vert", "./resources/shaders/normal.frag");

	proj = glm::perspective(glm::radians(90.f), (float)width / (float)height, 0.1f, 10000.f);

	
	glEnable(GL_DEPTH_TEST);


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
	//Tell attachments
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	//Create a depth buffer
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//Create Light Buffer
	glGenFramebuffers(1, &lightBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, lightBuffer);

	glGenTextures(1, &lightTex);
	glBindTexture(GL_TEXTURE_2D, lightTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightTex, 0);
	unsigned int light_attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, light_attachments);


	read_JSON("./data/scenes/scene.json");

	objects.push_back(Object(new Model(models[0])));

	//Configure lighting pass shader
	lightingPassShader.Use();
	lightingPassShader.SetInt("gPosition", 0);
	lightingPassShader.SetInt("gNormal", 1);
	lightingPassShader.SetInt("gAlbedoSpec", 2);


	//Create light
	scene_lights.push_back(Light{ vec3(0,25,0),vec3(1,1,1),1,0.007f,0.0011f,new Model(models[1]) });
	scene_lights.push_back(Light{ vec3(0,25,25),vec3(1,1,1),1,0.007f,0.0011f,new Model(models[1]) });
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
		//Optimization
		gBufferShader.Use();
		gBufferShader.SetMat4("projection", proj);
		gBufferShader.SetMat4("view", m_cam.ViewMatrix);

		for (auto obj : objects)
			obj.Draw(gBufferShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, lightBuffer);

		//Lighting Pass
		//Blending
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		lightingPassShader.Use();
		lightingPassShader.SetMat4("projection", proj);
		lightingPassShader.SetMat4("view", m_cam.ViewMatrix);
		lightingPassShader.SetVec3("viewPos", m_cam.camPos);
		lightingPassShader.SetFloat("Ambient", ambient);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

		
		for (unsigned int i = 0; i < scene_lights.size(); i++)
		{
			lightingPassShader.SetVec3("light.Position", scene_lights[i].position);
			lightingPassShader.SetVec3("light.Color", scene_lights[i].color);
			lightingPassShader.SetFloat("light.Constant", scene_lights[i].constant);
			lightingPassShader.SetFloat("light.Linear", scene_lights[i].linear);
			lightingPassShader.SetFloat("light.Quadratic", scene_lights[i].quadratic);

			renderQuad();
		}
		
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Render Lights
		shader.Use();
		shader.SetMat4("projection", proj);
		shader.SetMat4("view", m_cam.ViewMatrix);
		for (unsigned int i = 0; i < scene_lights.size(); i++)
		{
			/*mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, scene_lights[i].position);
			model = glm::scale(model, glm::vec3(1));
			shader.SetMat4("model", model);
			shader.SetVec3("lightColor", scene_lights[i].color);
			renderCube();*/
			scene_lights[i].model->transform.SetScale(glm::vec3(1));
			scene_lights[i].model->Draw(shader);
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