/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 13:33:29 by mbatty            #+#    #+#             */
/*   Updated: 2025/07/05 13:55:33 by mbatty           ###   ########.fr       */
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
float	SCREEN_WIDTH = 860;
float	SCREEN_HEIGHT = 520;
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
	if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
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
	shader->load({"cube", "shaders/cube.vs", "shaders/cube.fs"});

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

	static int frame = 0;
	static std::string	fps = "0 fps";
	std::string			particles_count;
	std::string			load_particles;

	if (frame++ >= currentFPS / 10)
	{
		frame = 0;
		fps = getFPSString();
	}
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
			pos = glm::vec3(0);
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);
		}
		Mesh(glm::vec3 pos): Mesh()
		{
			this->pos = pos;
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

			model = glm::mat4(1);

			model = glm::translate(model, pos);

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
		void generateCube()
		{
			vertices.clear();
			indices.clear();
		
			// Cube vertices (same as before)
			vertices = {
				{-0.5f, -0.5f, -0.5f}, // 0
				{ 0.5f, -0.5f, -0.5f}, // 1
				{ 0.5f,  0.5f, -0.5f}, // 2
				{-0.5f,  0.5f, -0.5f}, // 3
				{-0.5f, -0.5f,  0.5f}, // 4
				{ 0.5f, -0.5f,  0.5f}, // 5
				{ 0.5f,  0.5f,  0.5f}, // 6
				{-0.5f,  0.5f,  0.5f}  // 7
			};
		
			// Fixed clockwise winding for each face (when viewed from outside)
			indices = {
				// Back face
				0, 1, 2,
				2, 3, 0,
				// Front face
				6, 5, 4,
				4, 7, 6,
				// Left face
				7, 4, 0,
				0, 3, 7,
				// Right face
				1, 5, 6,
				6, 2, 1,
				// Bottom face
				0, 4, 5,
				5, 1, 0,
				// Top face
				3, 2, 6,
				6, 7, 3
			};
		}
		glm::vec3	getPos(){return (this->pos);}
		void	setPos(glm::vec3 pos){this->pos = pos;}
	private:
		std::vector<glm::vec3>	vertices;
		std::vector<unsigned int>	indices;
		glm::vec3		pos;
		unsigned int	VAO;
		unsigned int	VBO;
		unsigned int	EBO;
		glm::mat4		model;
};

std::vector<Mesh *>	terrainMeshes;
std::vector<Mesh *>	waterMeshes;
		
void	render()
{
	SKYBOX->draw(*ACTIVE_CAMERA, *SHADER_MANAGER->get("skybox"));

	TEXTURE_MANAGER->get("textures/moss_block.bmp")->use();
	TEXTURE_MANAGER->get("textures/stone.bmp")->use1();
	TEXTURE_MANAGER->get("textures/snow.bmp")->use2();
	for (Mesh *terrainMesh : terrainMeshes)
		if (glm::length(ACTIVE_CAMERA->pos - (terrainMesh->getPos() + glm::vec3(16, 0, 16))) < 320)
    		terrainMesh->draw(SHADER_MANAGER->get("mesh"));
	for (Mesh *terrainMesh : waterMeshes)
		if (glm::length(ACTIVE_CAMERA->pos - (terrainMesh->getPos() + glm::vec3(16, 0, 16))) < 320)
			terrainMesh->draw(SHADER_MANAGER->get("water"));
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

#include "FrameBuffer.hpp"

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

	consoleLog("Starting...", NORMAL);
	
	try {
		Engine	engine;

		consoleLog("Loading skybox", NORMAL);
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

		consoleLog("Generating terrain", NORMAL);
		terrainMeshes.reserve(32 * 32);
		waterMeshes.reserve(32 * 32);
		for (int i = 0; i < 32; i++)
		{
			for (int j = 0; j < 32; j++)
			{
				terrainMeshes.push_back(new Mesh(glm::vec3(i * 32, 0, j * 32)));
				terrainMeshes.back()->generatePlane(32, 32);
				terrainMeshes.back()->upload();
			}
		}
		for (int i = 0; i < 32; i++)
		{
			for (int j = 0; j < 32; j++)
			{
				waterMeshes.push_back(new Mesh(glm::vec3(i * 32, 0, j * 32)));
				waterMeshes.back()->generatePlane(32, 32);
				waterMeshes.back()->upload();
			}
		}

		consoleLog("Creating frame buffers", NORMAL);
		FrameBuffer	framebuffer;
		FrameBuffer	framebuffer2;

		lolTexID = framebuffer2.getTexture();

		ACTIVE_CAMERA = CAMERA;

		framebuffer2.resize(160, 90);
		framebuffer.resize(860, 520); //Lethal company size lol
		
		CAMERA->pos = glm::vec3(100.0, 30.0, 100.0);

		Camera	teste;
		teste.pos = glm::vec3(345.531, 26.906, 778.647);
		CAMERA->pos = glm::vec3(345.531, 26.906, 778.647);
		teste.pitch = -6.62461;
		teste.yaw = 24.7051;

		int	frame = 10;

		Mesh	player;
		player.generateCube();
		player.upload();

		consoleLog("Finished, starting rendering...", SUCCESS);
		while (WINDOW->up())
		{
			WINDOW->loopStart();
			CAMERA->update();
			teste.update();

			framebuffer.use();

			update(SHADER_MANAGER);
			update();
			
			render();

			player.setPos(teste.pos);
			ACTIVE_CAMERA->setViewMatrix(*SHADER_MANAGER->get("cube"));
			SHADER_MANAGER->get("cube")->setVec3("color", glm::vec3(1.0, 0.0, 0.0));
			player.draw(SHADER_MANAGER->get("cube"));

			if (frame ++ >= currentFPS / 16)
			{
				ACTIVE_CAMERA = &teste;
				framebuffer2.use();
				update(SHADER_MANAGER);
	
				render();
				player.setPos(CAMERA->pos);
				ACTIVE_CAMERA->setViewMatrix(*SHADER_MANAGER->get("cube"));
				SHADER_MANAGER->get("cube")->setVec3("color", glm::vec3(0.0, 1.0, 0.0));
				player.draw(SHADER_MANAGER->get("cube"));
	
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
		consoleLog("Program crashed: " + std::string(e.what()), LogSeverity::MELTDOWN);
		return (1);
	}
}
