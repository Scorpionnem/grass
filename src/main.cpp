/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 13:33:29 by mbatty            #+#    #+#             */
/*   Updated: 2025/07/09 16:38:08 by mbatty           ###   ########.fr       */
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
#include "FrameBuffer.hpp"

#define WORLD_SIZE 32

float	FOV = 65;
float	SCREEN_WIDTH = 860;
float	SCREEN_HEIGHT = 520;
float	RENDER_DISTANCE = 448;

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

//Allocation handled by Engine
FrameBuffer	*MAIN_FRAME_BUFFER;
//Allocation handled by Engine
FrameBuffer	*TERRAIN_DEPTH_BUFFER;
//Allocation handled by Engine
FrameBuffer	*WATER_DEPTH_BUFFER;

void	keyboard_input(GLFWwindow *window, unsigned int key)
{
	(void)window;(void)key;
}

void	key_hook(GLFWwindow *window, int key, int, int action, int)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && WINDOW->up())
	{
		consoleLog("Quitting game.", LogSeverity::WARNING);
		glfwSetWindowShouldClose(window, true);
	}
	if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
	{
		F3 = !F3;
		if (!F3)
			glfwSwapInterval(1);
		else
		{
			consoleLog("WARNING: Debug mode is ON, more resources will be used.", LogSeverity::WARNING);
			glfwSwapInterval(0);
		}
	}
}

void	build(ShaderManager *shader)
{
	consoleLog("Building shaders...", LogSeverity::NORMAL);
	shader->load({"text", TEXT_VERT_SHADER, TEXT_FRAG_SHADER});
	shader->load({"skybox", SKYBOX_VERT_SHADER, SKYBOX_FRAG_SHADER});
	shader->load({"mesh", "shaders/mesh.vs", "shaders/mesh.fs"});
	shader->load({"water", "shaders/water.vs", "shaders/water.fs"});
	shader->load({"post", "shaders/post.vs", "shaders/post.fs"});

	shader->load({"test", "shaders/test.vs", "shaders/test.fs"});
	shader->load({"cube", "shaders/cube.vs", "shaders/cube.fs"});
	shader->load({"grass", "shaders/grass.vs", "shaders/grass.fs"});

	Texture::use("tex0", 0, 0, shader->get("text"));

	Texture::use("skybox", 0, 0, shader->get("skybox"));

	Texture::use("grass_texture", 0, 0, shader->get("mesh"));
	Texture::use("stone_texture", 0, 1, shader->get("mesh"));
	Texture::use("snow_texture", 0, 2, shader->get("mesh"));
	Texture::use("depthTex", 0, 3, shader->get("mesh"));

	Texture::use("screenTexture", 0, 0, shader->get("post"));
	Texture::use("depthTex", 0, 1, shader->get("post"));

	Texture::use("screenTexture", 0, 0, shader->get("test"));

	Texture::use("depthTex", 0, 0, shader->get("water"));
	Texture::use("waterDepthTex", 0, 1, shader->get("water"));
	
	consoleLog("Finished building shaders", LogSeverity::SUCCESS);
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
	Shader	*postShader = shaders->get("post");

	textShader->use();
	textShader->setFloat("time", glfwGetTime());
	textShader->setFloat("SCREEN_WIDTH", SCREEN_WIDTH);
	textShader->setFloat("SCREEN_HEIGHT", SCREEN_HEIGHT);

	meshShader->use();
	ACTIVE_CAMERA->setViewMatrix(*meshShader);
	meshShader->setVec3("viewPos", ACTIVE_CAMERA->pos);
	meshShader->setFloat("time", glfwGetTime());
	meshShader->setFloat("RENDER_DISTANCE", RENDER_DISTANCE);

	waterShader->use();
	ACTIVE_CAMERA->setViewMatrix(*waterShader);
	waterShader->setVec3("viewPos", ACTIVE_CAMERA->pos);
	waterShader->setFloat("time", glfwGetTime());
	waterShader->setFloat("RENDER_DISTANCE", RENDER_DISTANCE);

	postShader->use();
	postShader->setFloat("RENDER_DISTANCE", RENDER_DISTANCE);
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
		CAMERA->pos = CAMERA->pos - glm::normalize(glm::cross(CAMERA->front, CAMERA->up)) * (cameraSpeed * speedBoost);
	if (glfwGetKey(window.getWindowData(), GLFW_KEY_D) == GLFW_PRESS)
		CAMERA->pos = CAMERA->pos + glm::normalize(glm::cross(CAMERA->front, CAMERA->up)) * (cameraSpeed * speedBoost);
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
		MAIN_FRAME_BUFFER = new FrameBuffer;
		TERRAIN_DEPTH_BUFFER = new FrameBuffer(FrameBufferType::DEPTH);
		WATER_DEPTH_BUFFER = new FrameBuffer(FrameBufferType::DEPTH);
	}
	~Engine()
	{
		delete MAIN_FRAME_BUFFER;
		delete TERRAIN_DEPTH_BUFFER;
		delete WATER_DEPTH_BUFFER;
		consoleLog("Done.", LogSeverity::NORMAL);
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
			scale = glm::vec3(1);
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

			model = glm::mat4(1.f);

			model = glm::translate(model, pos);

			model = glm::scale(model, scale);

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
		glm::vec3	getScale(){return (this->scale);}
		void	setPos(glm::vec3 pos){this->pos = pos;}
		void	setScale(glm::vec3 scale){this->scale = scale;}
	private:
		std::vector<glm::vec3>	vertices;
		std::vector<unsigned int>	indices;
		glm::vec3		pos;
		glm::vec3		scale;
		unsigned int	VAO;
		unsigned int	VBO;
		unsigned int	EBO;
		glm::mat4		model;
};

unsigned int	terrainDepthTex;
unsigned int	waterDepthTex;

void	render()
{
	SKYBOX->draw(*ACTIVE_CAMERA, *SHADER_MANAGER->get("skybox"));
}

void	update()
{
}

void	drawDepthTex(glm::vec3 offset, const unsigned int texture)
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	Shader	*test = SHADER_MANAGER->get("test");
	ACTIVE_CAMERA->setViewMatrix(*test);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(160 + 16, 128 + 16, 0.0f) * offset);
    model = glm::scale(model, glm::vec3(160, 128, 1.0f));

    glm::mat4 projection = glm::ortho(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
				
	test->setMat4("projection", projection);
	test->setMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

class	Chunk
{
	public:
		Chunk(glm::vec3 pos): terrain(pos), water(pos)
		{
			terrain.generatePlane(16.f, 16.f);
			terrain.setScale(glm::vec3(2, 1, 2));
			terrain.upload();
			water.generatePlane(16.f, 16.f);
			water.setScale(glm::vec3(2, 1, 2));
			water.upload();
			this->pos = pos;
		}
		~Chunk(){}
		void	drawTerrain(Shader *shader)
		{
			shader->use();
			TEXTURE_MANAGER->get("textures/moss_block.bmp")->use(0);
			TEXTURE_MANAGER->get("textures/stone.bmp")->use(1);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, TERRAIN_DEPTH_BUFFER->getTexture());
			terrain.draw(shader);
		}
		void	drawWater(Shader *shader)
		{
			shader->use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TERRAIN_DEPTH_BUFFER->getTexture());
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, WATER_DEPTH_BUFFER->getTexture());
			water.draw(shader);
		}
		glm::vec3	getPos() {return (this->pos);}
		bool		isInRange()
		{
			return (glm::length(ACTIVE_CAMERA->pos - (this->pos + glm::vec3(16.f, 0.f, 16.f))) < RENDER_DISTANCE - 64.f);
		}
		float		getDistance() const
		{
			return (glm::length(ACTIVE_CAMERA->pos - (this->pos + glm::vec3(16.f, 0.f, 16.f))));
		}
	private:
		Mesh	terrain;
		Mesh	water;
		glm::vec3	pos;
};

class	Region
{
	public:
		Region(glm::vec3 pos)
		{
			this->pos = pos;
			chunks.resize(4 * 4);
		}
		~Region()
		{
			for (auto *chunk : chunks)
				delete chunk;
		}
		void	generate()
		{
			int	chunk = 0;
			for (int x = 0; x < 4; x++)
				for (int y = 0; y < 4; y++)
					chunks[chunk++] = new Chunk((this->pos * glm::vec3(4.f) * glm::vec3(32.f)) + glm::vec3(x * 32.f, 0, y * 32.f));
		}
		void	drawTerrain(Shader *shader)
		{
			for (auto *chunk : visibleChunks)
				if (chunk->isInRange())
					chunk->drawTerrain(shader);
		}
		void	drawWater(Shader *shader)
		{
			sortChunks();
			for (auto *chunk : visibleChunks)
				if (chunk->isInRange())
					chunk->drawWater(shader);
		}
		void	drawBoth(Shader *terrainShader, Shader *waterShader)
		{
			sortChunks();
			for (auto *chunk : visibleChunks)
				if (chunk->isInRange())
				{
					chunk->drawTerrain(terrainShader);
					chunk->drawWater(waterShader);
				}
		}
		bool isInRange(const glm::vec3& cameraPos) const
		{
			glm::vec3 center = (this->pos * glm::vec3(4.f) * glm::vec3(32.f)) + glm::vec3(64.0f, 0.0f, 64.0f);
			float distance = glm::length(cameraPos - center);
			
			return (distance < RENDER_DISTANCE + 64.0f);
		}
		float	getDistance() const
		{
			glm::vec3 center = (this->pos * glm::vec3(4.f) * glm::vec3(32.f)) + glm::vec3(64.0f, 0.0f, 64.0f);
			float distance = glm::length(ACTIVE_CAMERA->pos - center);
			
			return (distance);
		}
		void	computeVisibility()
		{
			visibleChunks.clear();
			visibleChunks.reserve(chunks.size());
			for (auto *chunk : chunks)
				if (chunk->isInRange())
					visibleChunks.emplace_back(chunk);
			visibleChunks.shrink_to_fit();
			sortChunks();
		}
	private:
		void	sortChunks()
		{
			std::sort(visibleChunks.begin(), visibleChunks.end(),
			[](const Chunk *cp1, const Chunk *cp2)
			{
				return (cp1->getDistance() > cp2->getDistance());
			});
		}
		std::vector<Chunk *>	chunks;
		std::vector<Chunk *>	visibleChunks;
		glm::vec3				pos;
};

class	World
{
	public:
		World()
		{
			consoleLog("Generating terrain", NORMAL);
			for (uint x = 0; x < WORLD_SIZE; x++)
			{
				for (uint y = 0; y < WORLD_SIZE; y++)
				{
					regions.push_back(new Region(glm::vec3(x, 0, y)));
					regions.back()->generate();
				}
			}
			consoleLog("Generating terrain, done", SUCCESS);
		}
		~World()
		{
			visibleRegions.clear();
			for (auto *region : regions)
				delete region;
		}
		void	computeVisibility()
		{
			visibleRegions.clear();
			visibleRegions.reserve(regions.size());
			for (auto *region : regions)
				if (region->isInRange(CAMERA->pos))
				{
					region->computeVisibility();
					visibleRegions.emplace_back(region);
				}
			visibleRegions.shrink_to_fit();
			sortChunks();
		}
		void	drawTerrain()
		{
			for (auto *region : visibleRegions)
				region->drawTerrain(SHADER_MANAGER->get("mesh"));
		}
		void	drawWater()
		{
			for (auto *region : visibleRegions)
				region->drawWater(SHADER_MANAGER->get("water"));
		}
		void	drawBoth()
		{
			for (auto *region : visibleRegions)
				region->drawBoth(SHADER_MANAGER->get("mesh"), SHADER_MANAGER->get("water"));
		}
	private:
		void	sortChunks()
		{
			std::sort(visibleRegions.begin(), visibleRegions.end(),
			[](const Region *cp1, const Region *cp2)
			{
				return (cp1->getDistance() > cp2->getDistance());
			});
		}
		std::vector<Region *>	regions;
		std::vector<Region *>	visibleRegions;
};

int	main(void)
{
	consoleLog("Starting...", NORMAL);

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

		World	world;

		ACTIVE_CAMERA = CAMERA;

		// MAIN_FRAME_BUFFER->resize(860, 520); //Lethal company size lol
		// TERRAIN_DEPTH_BUFFER->resize(860, 520);
		// WATER_DEPTH_BUFFER->resize(860, 520);

		CAMERA->pos = glm::vec3(0, 0, 0);

		consoleLog("Starting rendering...", NORMAL);
		while (WINDOW->up())
		{
			WINDOW->loopStart();
			CAMERA->update();

			update(SHADER_MANAGER);
			update();
			
			MAIN_FRAME_BUFFER->resizeToWindow();
			WATER_DEPTH_BUFFER->resizeToWindow();
			TERRAIN_DEPTH_BUFFER->resizeToWindow();

			world.computeVisibility();

			TERRAIN_DEPTH_BUFFER->use();
			world.drawTerrain();

			WATER_DEPTH_BUFFER->use();
			glDisable(GL_CULL_FACE);
			world.drawWater();
			glEnable(GL_CULL_FACE);
			world.drawTerrain();

			MAIN_FRAME_BUFFER->use();
			render();

			world.drawBoth();

			FrameBuffer::reset();

			updatePostShader(SHADER_MANAGER);
			Texture::use(WATER_DEPTH_BUFFER->getTexture(), 1);
			FrameBuffer::drawFrame(SHADER_MANAGER->get("post"), MAIN_FRAME_BUFFER->getTexture());

			drawUI();

			if (F3)
			{
				drawDepthTex(glm::vec3(1, 1, 1), TERRAIN_DEPTH_BUFFER->getTexture());
				drawDepthTex(glm::vec3(3, 1, 3), WATER_DEPTH_BUFFER->getTexture());
			}

			frame_key_hook(*WINDOW);
			WINDOW->loopEnd();
		}
	} catch (const std::exception &e) {
		consoleLog("Program crashed: " + std::string(e.what()), LogSeverity::MELTDOWN);
		return (1);
	}
}
