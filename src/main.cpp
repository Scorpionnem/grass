/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 13:33:29 by mbatty            #+#    #+#             */
/*   Updated: 2025/07/04 12:15:14 by mbatty           ###   ########.fr       */
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

unsigned int	lolTexID;

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

Camera				*ACTIVE_CAMERA;

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
	
	shader->load({"test", "shaders/test.vs", "shaders/test.fs"});

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
	
	shader->get("test")->use();
	shader->get("test")->setInt("screenTexture", 0);
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

void	updatePostShader(ShaderManager *shaders)
{
	Shader	*postShader = shaders->get("post");

	postShader->use();
	postShader->setVec3("viewPos", ACTIVE_CAMERA->pos);
	postShader->setFloat("time", glfwGetTime());
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

	ACTIVE_CAMERA->setViewMatrix(*meshShader);
	meshShader->setVec3("viewPos", ACTIVE_CAMERA->pos);
	meshShader->setFloat("time", glfwGetTime());

	ACTIVE_CAMERA->setViewMatrix(*waterShader);
	waterShader->setVec3("viewPos", ACTIVE_CAMERA->pos);
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
	SKYBOX->draw(*ACTIVE_CAMERA, *SHADER_MANAGER->get("skybox"));

	TEXTURE_MANAGER->get("textures/moss_block.bmp")->use();
	TEXTURE_MANAGER->get("textures/stone.bmp")->use1();
	TEXTURE_MANAGER->get("textures/snow.bmp")->use2();
	mesh.draw(SHADER_MANAGER->get("mesh"));
	waterMesh.draw(SHADER_MANAGER->get("water"));
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
    // positions     // texCoords
    1.0f, -1.0f,      1.0f, 0.0f,
   -1.0f, -1.0f,      0.0f, 0.0f,
   -1.0f,  1.0f,      0.0f, 1.0f,

    1.0f,  1.0f,      1.0f, 1.0f,
    1.0f, -1.0f,      1.0f, 0.0f,
   -1.0f,  1.0f,      0.0f, 1.0f
};

unsigned int quadVAO = 0;
unsigned int quadVBO = 0;

class	FrameBuffer
{
	public:
		FrameBuffer()
		{
			glGenFramebuffers(1, &frameBufferID);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glBindTexture(GL_TEXTURE_2D, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

			glGenRenderbuffers(1, &RBO);
			glBindRenderbuffer(GL_RENDERBUFFER, RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
			
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			FrameBuffer::loadQuadModel();
			this->resizeToWindow();
		}
		~FrameBuffer()
		{
			glDeleteFramebuffers(1, &frameBufferID);
			glDeleteTextures(1, &textureID);
			glDeleteRenderbuffers(1, &RBO);
			if (quadVAO != 0)
				glDeleteVertexArrays(1, &quadVAO);
			if (quadVBO != 0)
				glDeleteBuffers(1, &quadVBO);

			quadVAO = 0;
			quadVBO = 0;
		}
		/*
			resizes the frame buffer, will also resize viewport when calling use() so dont forget to use reset
		*/
		void	resize(float width, float height)
		{
			this->width = width;
			this->height = height;
			glBindTexture(GL_TEXTURE_2D, this->textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		}
		void	resizeToWindow()
		{
			width = SCREEN_WIDTH;
			height = SCREEN_HEIGHT;
			glBindTexture(GL_TEXTURE_2D, this->textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
		}
		void	use()
		{
			SCREEN_WIDTH = width;
			SCREEN_HEIGHT = height;
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, this->frameBufferID);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
		}
		//Resets back to the main frame (The one drawn on screen)
		static void	reset()
		{
			int	width,height;
			glfwGetWindowSize(WINDOW->getWindowData(), &width, &height);
			SCREEN_WIDTH = width;
			SCREEN_HEIGHT = height;
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT);
		}
		static void	loadQuadModel()
		{
			if (quadVAO != 0 || quadVBO != 0)
				return ;
				
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
		}
		//Will draw the texture using the shader on the currently bound frame.
		static void	drawFrame(Shader *shader, unsigned int texture)
		{
			loadQuadModel();

			shader->use();
			glBindVertexArray(quadVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		unsigned int	getTexture()
		{
			return (textureID);
		}
	private:
		float	width;
		float	height;
		unsigned int frameBufferID;
		unsigned int textureID;
		unsigned int RBO;
};

void	drawNoPost()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	Shader	*test = SHADER_MANAGER->get("test");
	ACTIVE_CAMERA->setViewMatrix(*test);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(160 + 16, 128 + 16, 0.0f));
    model = glm::scale(model, glm::vec3(160, 128, 1.0f));

    glm::mat4 projection = glm::ortho(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
				
	test->setMat4("projection", projection);
	test->setMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lolTexID);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

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

		FrameBuffer	framebuffer;

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

		CAMERA->pos = glm::vec3(100.0, 30.0, 100.0);

		FrameBuffer	framebuffer2;

		lolTexID = framebuffer2.getTexture();

		Camera	teste;
		teste.pos = glm::vec3(345.531, 26.906, 778.647);
		teste.pitch = -6.62461;
		teste.yaw = 24.7051;

		ACTIVE_CAMERA = CAMERA;

		framebuffer2.resize(160, 90);
		framebuffer.resize(640, 360); //Lethal company size lol

		int	frame = 10;

		glfwSwapInterval(0);

		while (WINDOW->up())
		{
			WINDOW->loopStart();
			CAMERA->update();
			teste.update();

			framebuffer.use();

			update(SHADER_MANAGER);
			update();
			
			render(mesh, waterMesh);

			if (frame ++ >= currentFPS / 8)
			{
				ACTIVE_CAMERA = &teste;
				framebuffer2.use();
				update(SHADER_MANAGER);
	
				render(mesh, waterMesh);
	
				ACTIVE_CAMERA = CAMERA;
				frame = 0;
			}
			teste.yaw = 24.7051 + cos(glfwGetTime()) * 10;

			FrameBuffer::reset();

			updatePostShader(SHADER_MANAGER);
			FrameBuffer::drawFrame(SHADER_MANAGER->get("post"), framebuffer.getTexture());

			drawUI();

			drawNoPost();

			frame_key_hook(*WINDOW);
			WINDOW->loopEnd();
		}
	} catch (const std::exception &e) {
		std::cerr << "An error occurred: " << e.what() << std::endl;
		return (1);
	}
}
