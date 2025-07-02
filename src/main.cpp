/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 13:33:29 by mbatty            #+#    #+#             */
/*   Updated: 2025/06/25 11:56:33 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libs.hpp"
#include "Font.hpp"
#include "ShaderManager.hpp"
#include "TextureManager.hpp"
#include "Camera.hpp"
#include "Window.hpp"
#include "Texture.hpp"
#include "Skybox.hpp"

float	FOV = 65;
float	SCREEN_WIDTH = 800;
float	SCREEN_HEIGHT = 800;
float	RENDER_DISTANCE = 1000;

bool	F3 = false;
bool	PAUSED = false;
bool	SKYBOX_ACTIVE = false;

int		currentFPS = 60;

Font				*FONT;
Window				*WINDOW;
Camera				*CAMERA;
Skybox				*SKYBOX;

TextureManager		*TEXTURE_MANAGER;

ShaderManager		*SHADER_MANAGER;

float				lastX = SCREEN_WIDTH / 2;
float				lastY = SCREEN_HEIGHT / 2;

void	keyboard_input(GLFWwindow *window, unsigned int key)
{
	(void)window;(void)key;
}

void	key_hook(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	(void)window;(void)key;(void)scancode;(void)action;(void)mods;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		F3 = !F3;
		if (!F3)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
	}
		
}

void	build(ShaderManager *shader)
{
	shader->load({"text", TEXT_VERT_SHADER, TEXT_FRAG_SHADER});
	shader->load({"skybox", SKYBOX_VERT_SHADER, SKYBOX_FRAG_SHADER});
	shader->load({"mesh", "shaders/mesh.vs", "shaders/mesh.fs"});
	shader->load({"water", "shaders/water.vs", "shaders/water.fs"});
	shader->load({"post", "shaders/post.vs", "shaders/post.fs"});

	shader->get("text")->use();
	shader->get("text")->setInt("tex0", 0);
	shader->get("skybox")->use();
	shader->get("skybox")->setInt("skybox", 0);
	shader->get("mesh")->use();
	shader->get("mesh")->setInt("grass_texture", 0);
	shader->get("mesh")->setInt("stone_texture", 1);
	shader->get("mesh")->setInt("snow_texture", 2);
	shader->get("post")->use();
	shader->get("post")->setInt("screenTexture", 0);
}

std::string	getFPSString()
{
	currentFPS = (int)(1.0f / WINDOW->_deltaTime);
	return (std::to_string(currentFPS) + " fps");
}

void	drawUI()
{
	glDisable(GL_DEPTH_TEST);

	static std::string	fps = "0 fps";
	std::string			particles_count;
	std::string			load_particles;

	if ((int)WINDOW->_lastFrame != (int)WINDOW->_currentFrame)
		fps = getFPSString();
	FONT->putString(fps, *SHADER_MANAGER->get("text"),
		glm::vec2((SCREEN_WIDTH / 2) - (fps.length() * 15) / 2, 0),
		glm::vec2(fps.length() * 15, 15));
	
	glEnable(GL_DEPTH_TEST);
}

void	update(ShaderManager *shaders)
{
	Shader	*textShader = shaders->get("text");
	Shader	*meshShader = shaders->get("mesh");
	Shader	*waterShader = shaders->get("water");

	textShader->use();
	textShader->setFloat("time", glfwGetTime());
	textShader->setFloat("SCREEN_WIDTH", SCREEN_WIDTH);
	textShader->setFloat("SCREEN_HEIGHT", SCREEN_HEIGHT);
	CAMERA->setViewMatrix(*meshShader);
	meshShader->setVec3("viewPos", CAMERA->pos);
	meshShader->setFloat("time", glfwGetTime());
	CAMERA->setViewMatrix(*waterShader);
	waterShader->setVec3("viewPos", CAMERA->pos);
	waterShader->setFloat("time", glfwGetTime());
}

void	frame_key_hook(Window &window)
{
	float cameraSpeed = 15.0f * window.getDeltaTime();
	float	speedBoost = 1.0f;

	if (glfwGetKey(window.getWindowData(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		speedBoost = 20.0f;
	
	if (glfwGetKey(window.getWindowData(), GLFW_KEY_W) == GLFW_PRESS)
		CAMERA->pos = CAMERA->pos + CAMERA->front * (cameraSpeed * speedBoost);
	if (glfwGetKey(window.getWindowData(), GLFW_KEY_S) == GLFW_PRESS)
		CAMERA->pos = CAMERA->pos - CAMERA->front * (cameraSpeed * speedBoost);
	if (glfwGetKey(window.getWindowData(), GLFW_KEY_SPACE) == GLFW_PRESS)
		CAMERA->pos = CAMERA->pos + CAMERA->up * (cameraSpeed * speedBoost);
	if (glfwGetKey(window.getWindowData(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		CAMERA->pos = CAMERA->pos - CAMERA->up * (cameraSpeed * speedBoost);
		
	if (glfwGetKey(window.getWindowData(), GLFW_KEY_A) == GLFW_PRESS)
		CAMERA->pos = CAMERA->pos - glm::cross(normalize(CAMERA->front), normalize(CAMERA->up)) * (cameraSpeed * speedBoost);
	if (glfwGetKey(window.getWindowData(), GLFW_KEY_D) == GLFW_PRESS)
		CAMERA->pos = CAMERA->pos + glm::cross(normalize(CAMERA->front), normalize(CAMERA->up)) * (cameraSpeed * speedBoost);
}

void	move_mouse_hook(GLFWwindow* window, double xpos, double ypos)
{
	(void)window;

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.1f;
	
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	
	CAMERA->yaw += xoffset;
	CAMERA->pitch += yoffset;
	
	if(CAMERA->pitch > 89.0f)
		CAMERA->pitch = 89.0f;
	if(CAMERA->pitch < -89.0f)
		CAMERA->pitch = -89.0f;
}

struct	Engine
{
	Engine()
	{
		WINDOW = &this->window;
		CAMERA = &this->camera;
		FONT = &this->font;
		SHADER_MANAGER = &this->shaderManager;
		build(SHADER_MANAGER);
		TEXTURE_MANAGER = &this->textureManager;
	}
	Window				window;
	Camera				camera;
	Font				font;
	ShaderManager		shaderManager;
	TextureManager		textureManager;
};

class	Mesh
{
	public:
		Mesh()
		{
			model = glm::mat4(1.0);
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);
		}
		~Mesh()
		{
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
		}
		void	addVertice(const glm::vec3 &pos)
		{
			vertices.push_back(pos);
		}
		void	addIndex(unsigned int index)
		{
			indices.push_back(index);
		}
		void	upload()
		{
			glBindVertexArray(VAO);
		
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
		
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		void	draw(Shader	*shader)
		{
			shader->use();
			shader->setMat4("model", model);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		void generatePlane(int sizeX, int sizeZ)
		{
			vertices.clear();
			indices.clear();

			for (int z = 0; z <= sizeZ; z++)
				for (int x = 0; x <= sizeX; x++)
					vertices.emplace_back(glm::vec3(x, 0, z));

			for (int z = 0; z < sizeZ; z++)
			{
				for (int x = 0; x < sizeX; x++)
				{
					int start = z * (sizeX + 1) + x;
					int nextRow = start + sizeX + 1;

					// Top triangle
					addIndex(start);
					addIndex(start + 1);
					addIndex(nextRow);

					// Bottom triangle
					addIndex(start + 1);
					addIndex(nextRow + 1);
					addIndex(nextRow);
				}
			}
		}
	private:
		std::vector<glm::vec3>	vertices;
		std::vector<unsigned int>	indices;
		unsigned int	VAO;
		unsigned int	VBO;
		unsigned int	EBO;
		glm::mat4		model;
};

void	render(Mesh &mesh, Mesh &waterMesh)
{
	// SKYBOX->draw(*CAMERA, *SHADER_MANAGER->get("skybox"));

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	TEXTURE_MANAGER->get("textures/moss_block.bmp")->use();
	TEXTURE_MANAGER->get("textures/stone.bmp")->use1();
	TEXTURE_MANAGER->get("textures/snow.bmp")->use2();
	mesh.draw(SHADER_MANAGER->get("mesh"));
	waterMesh.draw(SHADER_MANAGER->get("water"));
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void	update()
{
}

#include <csignal>
#include <fontconfig/fontconfig.h>

void	handleSIGINT(int)
{
	glfwSetWindowShouldClose(WINDOW->getWindowData(), true);
}

float quadVertices[] = {
// positions // texCoords
-1.0f, 1.0f, 0.0f, 1.0f,
-1.0f, -1.0f, 0.0f, 0.0f,
1.0f, -1.0f, 1.0f, 0.0f,
-1.0f, 1.0f, 0.0f, 1.0f,
1.0f, -1.0f, 1.0f, 0.0f,
1.0f, 1.0f, 1.0f, 1.0f
};

int	main(int ac, char **av)
{
	FcInit();
	(void)ac;(void)av;
	std::signal(SIGINT, handleSIGINT);

	try {
		Engine	engine;

		Skybox	skybox({SKYBOX_PATHES});
		SKYBOX = &skybox;

		CAMERA->pos.z = 0;

		{
			double	x,y;
			glfwSetInputMode(WINDOW->getWindowData(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwGetCursorPos(WINDOW->getWindowData(), &x, &y);
			lastX = x;
			lastY = y;
		}

		Mesh	mesh;
		Mesh	waterMesh;

		unsigned int	mapSize = 1000;

		mesh.generatePlane(mapSize, mapSize);
		waterMesh.generatePlane(mapSize, mapSize);

		mesh.upload();
		waterMesh.upload();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		unsigned int framebuffer;
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		// generate texture
		unsigned int texColorBuffer;
		glGenTextures(1, &texColorBuffer);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		// attach it to currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		unsigned int quadVAO;
		unsigned int quadVBO;

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
	
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		
		glBindVertexArray(0);

		while (WINDOW->up())
		{
			WINDOW->loopStart();
			CAMERA->update();
			update(SHADER_MANAGER);

			update();

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			render(mesh, waterMesh);
			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT);
			SHADER_MANAGER->get("post")->use();
			glBindVertexArray(quadVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			drawUI();

			frame_key_hook(*WINDOW);
			WINDOW->loopEnd();
		}
	} catch (const std::exception &e) {
		std::cerr << "An error occurred: " << e.what() << std::endl;
		return (1);
	}
}
