#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GL/GL.h>
#include <SDL2/SDL.h>

#include <GL/glew.h>
#include <GL/GL.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "GLSLProgram.hpp"
#include "MeshData.h"

static int winID;
static GLsizei WIDTH = 1280;
static GLsizei HEIGHT = 720;


GLSLProgram shaderProgram;

bool wireframe = false;

const char * meshName = "meshes/sphere_8_4.obj";

class Renderable
{
public:
	glm::mat4 modelMat;
	MeshData mesh;
	GLSLProgram * program;
};

void InitializeProgram()
{
	if (!shaderProgram.compileShaderFromFile("shaders/TessMesh.vs", GLSLShader::VERTEX))
	{
		std::cout << shaderProgram.log();
		throw;
	}
	if (!shaderProgram.compileShaderFromFile("shaders/TessMesh.tcs", GLSLShader::TESS_CONTROL))
	{
		std::cout << shaderProgram.log();
		throw;
	}
	if (!shaderProgram.compileShaderFromFile("shaders/TessMesh.tes", GLSLShader::TESS_EVALUATION))
	{
		std::cout << shaderProgram.log();
		throw;
	}
	if (!shaderProgram.compileShaderFromFile("shaders/TessMesh.fs", GLSLShader::FRAGMENT))
	{
		std::cout << shaderProgram.log();
		throw;
	}

	if(!shaderProgram.link())
		throw shaderProgram.log();
}

void Init()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	
	InitializeProgram();	
}

void DrawScene(	const std::vector<Renderable *> & renderables,
				const glm::mat4 & projMat, const glm::mat4 & camMat)
{
	// Render every mesh with the correspoding model matrix
	for (unsigned i = 0; i < renderables.size(); i++)
	{
		// Bind the glsl program and this object's VAO
		renderables[i]->program->use();
		renderables[i]->mesh.Use();

		renderables[i]->program->setUniform("M", renderables[i]->modelMat);
		renderables[i]->program->setUniform("V", camMat);
		renderables[i]->program->setUniform("P", projMat);

		// Draw
		if(wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawElements(GL_PATCHES, renderables[i]->mesh.IndexCount(), GL_UNSIGNED_SHORT, 0);
	}

	// Unbind
	glBindVertexArray(0);
	glUseProgram(0);

}

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;
	}

	int imgFlags = IMG_INIT_PNG;
	if (!IMG_Init(imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		exit(1);
	}

	SDL_Window * window = SDL_CreateWindow("CS562", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if (window == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GLContext context_ = SDL_GL_CreateContext(window);
	if (context_ == nullptr)
	{
		SDL_DestroyWindow(window);
		std::cout << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		SDL_GL_DeleteContext(context_);
		SDL_DestroyWindow(window);
		std::cout << "GLEW Error: Failed to init" << std::endl;
		SDL_Quit();
		exit(1);
	}
	
	Init();

	// Camera data
	glm::vec3 camPos = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 camTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	float fovY = 60.0f;
	float near = 0.1f;
	float far = 100.0f;
	glm::mat4 camMat = glm::lookAt(camPos, camTarget, camUp);
	glm::mat4 projMat = glm::perspective(fovY, static_cast<float>(WIDTH) / HEIGHT, near, far);

	// Scene setup
	std::vector<Renderable *> renderables;
	
	Renderable * rend = new Renderable();
	rend->mesh.LoadOBJ(meshName);
	rend->mesh.BindBuffers();
	rend->modelMat = glm::mat4(1.0f);
	rend->program = &shaderProgram;
	renderables.push_back(rend);
	
	const float PI = 3.14159f;
	float camSpeed = 0.1f;
	float radius = 2.0f;
	float anglePhi = PI / 4.0f;
	float angleTheta = 0.0f;
	SDL_Event event;
	bool quit = false;

	bool spacePressed = false;
	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.scancode)
				{
				case SDL_SCANCODE_W:
					anglePhi -= camSpeed;
					break;
				case SDL_SCANCODE_S:
					anglePhi += camSpeed;
					break;
				case SDL_SCANCODE_D:
					angleTheta += camSpeed;
					break;
				case SDL_SCANCODE_A:
					angleTheta -= camSpeed;
					break;
				case SDL_SCANCODE_E:
					radius -= camSpeed;
					break;
				case SDL_SCANCODE_Q:
					radius += camSpeed;
					break;
				case SDL_SCANCODE_1:
					wireframe = !wireframe;
					break;
				case SDL_SCANCODE_ESCAPE:
					quit = true;
					break;
				case SDL_SCANCODE_SPACE:
					spacePressed = true;
					break;
				default:
					break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.scancode)
				{
				case SDL_SCANCODE_SPACE:
					spacePressed = false;
					break;
				}
				break;
			}
		}

		if (spacePressed)
		{
			shaderProgram = GLSLProgram();
			InitializeProgram();
		}

		// Compute regular camera
		float cosTheta = glm::cos(angleTheta);
		float sinTheta = glm::sin(angleTheta);
		float cosPhi = glm::cos(anglePhi);
		float sinPhi = glm::sin(anglePhi);
		
		camPos.z = cosTheta * sinPhi * radius;
		camPos.y = cosPhi * radius;
		camPos.x = sinTheta * sinPhi * radius;

		// Compute regular camera matrix
		camMat = glm::lookAt(camPos, camTarget, camUp);

		// Bind created FBO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// Clear framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glEnable(GL_DEPTH_TEST);

		DrawScene(renderables, projMat, camMat);
			   
		SDL_GL_SwapWindow(window);

	}

	for (size_t i = 0; i < renderables.size(); i++)
	{
		delete renderables[i];
	}

	SDL_GL_DeleteContext(context_);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}