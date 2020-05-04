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
#include <glm/gtc/random.hpp>

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
	renderImGUI();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
/**
* @brief 	Render ImGUI
*/
void Renderer::renderImGUI()
{

	ImGui::Begin("Fur and Shells", nullptr, m_flags);

	ImGui::Checkbox("Orbital", &m_cam.orbital);

	ImGui::Checkbox("Show Shells", &showShells);
	ImGui::Checkbox("Show Fins", &showFins);

	ImGui::SliderInt("Number of Shells", &numShells, 1, 16);
	ImGui::DragFloat("Shell Increment", &shellIncrement, 0.001f, 0.0001f,100.f);
	ImGui::SliderFloat("Max Opacity (For Fins)", &maxOpacity, 0.1f, 1.f);
	ImGui::InputFloat3("Comb Vector", &combVector.x);
	ImGui::DragFloat("Comb Strength", &combStrength, 0.1f, 0.1f,100.f);

	
	const char * items1[3] = {"cat","bunny","sphere"};
	ImGui::Combo("Model", &currentModel, items1, 3);

	const char * items2[9] = { "cat","grass","white fur", "leopard fur", "brown fur",
	"doted fur", "stripped fur", "white stripped fur", "purple fur" };
	ImGui::Combo("Texture", &currentTexture, items2, 9);

	ImGui::Checkbox("Show Example", &showSpheres);
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
	window = glfwCreateWindow(1920, 1080, "Furry", NULL, NULL);
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

	std::string fur[3] = { "./data/shaders/Fur/fins.vert", "./data/shaders/Fur/fins.geo",
		"./data/shaders/Fur/fins.frag"};

	shader = Shader("./data/shaders/Fur/normal.vert", "./data/shaders/Fur/normal.frag");
	furShader = Shader("./data/shaders/Fur/fur.vert", "./data/shaders/Fur/fur.frag");
	finShader = Shader(fur);

	proj = glm::perspective(glm::radians(90.f), (float)width / (float)height, 0.1f, 1000.f);
	
	read_JSON("./data/scenes/scene.json");

	//Furry Stuff
	LoadFur();
	models.push_back(models[2]);
	models.back().transform.SetPosition(vec3(-150, 0, 0));
	models.push_back(models[2]);
	models.back().transform.SetPosition(vec3(-75, 0, 0));
	models.push_back(models[2]);
	models.back().transform.SetPosition(vec3(0,0,0));
	models.push_back(models[2]);
	models.back().transform.SetPosition(vec3(75, 0, 0));
	models.push_back(models[2]);
	models.back().transform.SetPosition(vec3(150, 0, 0));
	



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

		glDepthMask(GL_TRUE);
		glClearColor(0.20f, 0.20f, 0.20f, 1.0f);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (showSpheres)
		{
			for (int i = 0; i < 5; i++)
			{
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
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureArray[i + 4]);

				models[3 + i].Draw(shader, false);

				//Render Fins
				if (showFins)
				{
					glEnable(GL_BLEND);
					glBlendEquation(GL_FUNC_ADD);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glDisable(GL_CULL_FACE);
					glEnable(GL_DEPTH_TEST);
					glDepthMask(GL_FALSE);
					glDepthFunc(GL_LESS);

					finShader.Use();
					finShader.SetMat4("proj", proj);
					finShader.SetMat4("view", m_cam.ViewMatrix);
					finShader.SetVec3("Eye", vec3(glm::inverse(models[3 + i].transform.M2W) * vec4(m_cam.camPos, 1)));
					finShader.SetMat4("MVP", proj * m_cam.ViewMatrix * models[3 + i].transform.M2W);
					finShader.SetVec3("combVector", combVector);
					finShader.SetFloat("combStrength", combStrength);
					finShader.SetInt("numShells", numShells);
					finShader.SetFloat("shellIncrement", shellIncrement);
					finShader.SetFloat("maxOpacity", maxOpacity);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, textureArray[i + 4]);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, finTexture);
					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, finOffset);

					models[3 + i].Draw(finShader, true);

				}


				//Render Shells
				if (showShells)
				{
					//glDepthMask(GL_FALSE);
					glEnable(GL_BLEND);
					glBlendEquation(GL_FUNC_ADD);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

					glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK);

					glEnable(GL_DEPTH_TEST);
					glDepthMask(GL_FALSE);
					glDepthFunc(GL_LESS);
					furShader.Use();
					furShader.SetMat4("proj", proj);
					furShader.SetMat4("view", m_cam.ViewMatrix);
					furShader.SetVec3("Eye", vec3(glm::inverse(models[currentModel].transform.M2W) * vec4(m_cam.camPos, 1)));
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, textureArray[i + 4]);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D_ARRAY, furTextureArray);
					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D_ARRAY, furTextureOffsetArray);

					furShader.SetVec3("combVector", combVector);
					furShader.SetFloat("combStrength", combStrength);
					furShader.SetInt("numShells", numShells);
					furShader.SetFloat("shellIncrement", shellIncrement);

					for (int j = 1; j <= numShells; j++)
					{
						furShader.SetInt("shell", j);
						models[3 + i].Draw(furShader);
					}
				}
			}

			
		}
		else
		{
			glDisable(GL_BLEND);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			//Render Mesh

			shader.Use();
			shader.SetMat4("proj", proj);
			shader.SetMat4("view", m_cam.ViewMatrix);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureArray[currentTexture]);

			models[currentModel].Draw(shader, false);

			//Render Fins
			if (showFins)
			{
				glEnable(GL_BLEND);
				glBlendEquation(GL_FUNC_ADD);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDisable(GL_CULL_FACE);
				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);
				glDepthFunc(GL_LESS);

				finShader.Use();
				finShader.SetMat4("proj", proj);
				finShader.SetMat4("view", m_cam.ViewMatrix);
				finShader.SetVec3("Eye", vec3(glm::inverse(models[currentModel].transform.M2W) * vec4(m_cam.camPos, 1)));
				finShader.SetMat4("MVP", proj * m_cam.ViewMatrix * models[currentModel].transform.M2W);
				finShader.SetVec3("combVector", combVector);
				finShader.SetFloat("combStrength", combStrength);
				finShader.SetInt("numShells", numShells);
				finShader.SetFloat("shellIncrement", shellIncrement);
				finShader.SetFloat("maxOpacity", maxOpacity);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureArray[currentTexture]);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, finTexture);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, finOffset);

				models[currentModel].Draw(finShader, true);

			}


			//Render Shells
			if (showShells)
			{
				//glDepthMask(GL_FALSE);
				glEnable(GL_BLEND);
				glBlendEquation(GL_FUNC_ADD);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);

				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);
				glDepthFunc(GL_LESS);
				furShader.Use();
				furShader.SetMat4("proj", proj);
				furShader.SetMat4("view", m_cam.ViewMatrix);
				furShader.SetVec3("Eye", vec3(glm::inverse(models[currentModel].transform.M2W) * vec4(m_cam.camPos, 1)));
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureArray[currentTexture]);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D_ARRAY, furTextureArray);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D_ARRAY, furTextureOffsetArray);

				furShader.SetVec3("combVector", combVector);
				furShader.SetFloat("combStrength", combStrength);
				furShader.SetInt("numShells", numShells);
				furShader.SetFloat("shellIncrement", shellIncrement);

				for (int i = 1; i <= numShells; i++)
				{
					furShader.SetInt("shell", i);
					models[currentModel].Draw(furShader);
				}




			}
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
	std::string fur[3] = { "./data/shaders/Fur/fins.vert", "./data/shaders/Fur/fins.geo",
		"./data/shaders/Fur/fins.frag" };

	shader = Shader("./data/shaders/Fur/normal.vert", "./data/shaders/Fur/normal.frag");
	furShader = Shader("./data/shaders/Fur/fur.vert", "./data/shaders/Fur/fur.frag");
	finShader = Shader(fur);

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

}

/*
	Load fur textures and additional data
*/

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

	
	unsigned int tex;
	TextureFromFile("./data/FurTexture/catColor.png", tex);
	textureArray.push_back(tex);
	TextureFromFile("./data/FurTexture/MainTextures/grass.jpg", tex);
	textureArray.push_back(tex);
	TextureFromFile("./data/FurTexture/MainTextures/whitefur.jpg", tex);
	textureArray.push_back(tex);
	TextureFromFile("./data/FurTexture/MainTextures/fur.jpg", tex);
	textureArray.push_back(tex);
	TextureFromFile("./data/FurTexture/MainTextures/fur_01.jpg", tex);
	textureArray.push_back(tex);
	TextureFromFile("./data/FurTexture/MainTextures/fur_02.jpg", tex);
	textureArray.push_back(tex);
	TextureFromFile("./data/FurTexture/MainTextures/fur_03.jpg", tex);
	textureArray.push_back(tex);
	TextureFromFile("./data/FurTexture/MainTextures/fur_04.jpg", tex);
	textureArray.push_back(tex);
	TextureFromFile("./data/FurTexture/MainTextures/fur_05.jpg", tex);
	textureArray.push_back(tex);
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