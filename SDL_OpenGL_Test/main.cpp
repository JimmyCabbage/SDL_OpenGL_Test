#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "stb_image.h"
#include "camera.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <algorithm>

//timing
using Time = std::chrono::steady_clock;
using float_time_point = std::chrono::time_point<Time, std::chrono::duration<float>>;

float_time_point getCurrentTime()
{
	return Time::now();
}

int main(int argc, char** argv)
{
	const auto startTime = getCurrentTime();

	SDL_Window* gWindow = nullptr;
	SDL_GLContext gContext;

	if (SDL_Init(SDL_INIT_VIDEO) > 0)
	{
		std::cerr << "Could not initialize SDL2: " << SDL_GetError() << std::endl;
		return 1;
	}

	gWindow = SDL_CreateWindow("OpenGL 3 SDL2 Testing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	gContext = SDL_GL_CreateContext(gWindow);
	glewInit();

	glViewport(0, 0, 800, 600);
	glEnable(GL_DEPTH_TEST);

	SDL_GL_LoadLibrary(NULL);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

	SDL_GL_SetSwapInterval(1);

	Shader basic_shader("shaders/vertex.glsl", "shaders/fragment.glsl");
	Shader lightingShader("shaders/lightingvertex.glsl", "shaders/lightingfragment.glsl");

	bool quit = false;
	SDL_Event ev;

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};
	/*
	float vertices[] = {
		// positions			// texture coords
		 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f    // top left
	};
	unsigned indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	*/

	unsigned VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned lightVAO;
	glGenVertexArrays(1, &lightVAO);

	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	Camera camera(glm::vec3(0.0, 0.0, 3.0), 600, 800);

	//matrixes
	glm::mat4 model = glm::mat4(1.0);

	glm::mat4 view = camera.ViewMat();

	glm::mat4 proj = camera.ProjectionMat();

	bool wbutton = false;

	bool wsad[4] = { 0,0,0,0 };

	float deltaTime = 0.0f, lastFrame = 0.0f;

	while (!quit)
	{
		float currentFrame = getCurrentTime().time_since_epoch().count();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		const float cameraSpeed = 2.5f * deltaTime;
		while (SDL_PollEvent(&ev))
			switch (ev.type)
			{
			case SDL_MOUSEWHEEL:
				if (ev.wheel.y > 0 || ev.wheel.y < 0)
				{
					camera.ChangeFov(ev.wheel.y, true);
				}
				break;
			case SDL_QUIT: quit = true; break;
			case SDL_KEYUP:
			case SDL_KEYDOWN:
				switch (ev.key.keysym.sym)
				{
				case 'v': wbutton = ev.type == SDL_KEYDOWN; break;
				case 'w': wsad[0] = ev.type == SDL_KEYDOWN; break;
				case 's': wsad[1] = ev.type == SDL_KEYDOWN; break;
				case 'a': wsad[2] = ev.type == SDL_KEYDOWN; break;
				case 'd': wsad[3] = ev.type == SDL_KEYDOWN; break;
				case SDLK_ESCAPE: quit = true; break;
				}
				break;
			}
		if (wbutton)	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		camera.MoveCamera(wsad[0], wsad[1], wsad[2], wsad[3], deltaTime);

		int x, y;
		SDL_GetRelativeMouseState(&x, &y);
		camera.RotateCamera(x, y);

		glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		basic_shader.use();
		basic_shader.setUniformVec3("objectColor", 1.0f, 0.5f, 0.31f);
		basic_shader.setUniformVec3("lightColor", 1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		view = camera.ViewMat();
		proj = camera.ProjectionMat();
		basic_shader.setUniform4fv("model", model);
		basic_shader.setUniform4fv("view", view);
		basic_shader.setUniform4fv("projection", proj);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lightingShader.use();
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightingShader.setUniform4fv("view", view);
		lightingShader.setUniform4fv("model", model);
		lightingShader.setUniform4fv("projection", proj);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		SDL_GL_SwapWindow(gWindow);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	SDL_GL_DeleteContext(gContext);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	SDL_Quit();

	return 0;
}