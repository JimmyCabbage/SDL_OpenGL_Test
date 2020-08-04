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

	bool quit = false;
	SDL_Event ev;

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
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
	//preload textures
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	auto containert = std::async(std::launch::async, stbi_load, "container.jpg", &width, &height, &nrChannels, 0);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int texture1, texture2;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Camera camera(glm::vec3(0.0, 0.0, 3.0), 600, 800);

	//matrixes
	glm::mat4 model = glm::mat4(1.0);

	glm::mat4 view = glm::mat4(1.0);

	glm::mat4 proj = camera.ProjectionMat();

	unsigned char* data = containert.get();
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Could not load texture\n";
	}
	stbi_image_free(data);

	glGenTextures(1, &texture2);

	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Could not load texture\n";
	}
	stbi_image_free(data);

	bool wbutton = false;

	basic_shader.use();
	glUniform1i(basic_shader.getUniformLoc("texture1"), 0);
	glUniform1i(basic_shader.getUniformLoc("texture2"), 1);

	unsigned modelLoc = basic_shader.getUniformLoc("model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	unsigned viewLoc = basic_shader.getUniformLoc("view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	unsigned projectionLoc = basic_shader.getUniformLoc("projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));

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

		view = camera.ViewMat();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		proj = camera.ProjectionMat();
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));

		glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		basic_shader.use();
		glBindVertexArray(VAO);

		for (unsigned i = 0; i < 10; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, cubePositions[i]);
			float angle = i ? (20.0f * i * ((getCurrentTime() - startTime).count())) : (1.0f * ((getCurrentTime() - startTime).count()));
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0, 0.3, 0.5));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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