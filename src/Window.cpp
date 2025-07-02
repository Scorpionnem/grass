/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Window.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 12:11:45 by mbatty            #+#    #+#             */
/*   Updated: 2025/06/23 10:57:38 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Window.hpp"
#include "Camera.hpp"

static void resize_hook(GLFWwindow* window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void	key_hook(GLFWwindow *window, int key, int scancode, int action, int mods);
void	keyboard_input(GLFWwindow *window, unsigned int key);
void	move_mouse_hook(GLFWwindow* window, double xpos, double ypos);

Window::Window() : _lastFrame(0)
{
	if (DEBUG)
		std::cout << "Creating window: " << WIN_NAME << std::endl;
	//Inits GLFW settings
	if (!glfwInit())
		throw std::runtime_error("Failed to initialize glfw");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Creates and opens window
	_windowData = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WIN_NAME, NULL, NULL);
	if (!_windowData)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create window");
	}
	glfwMakeContextCurrent(_windowData);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		throw std::runtime_error("Failed to init GLAD");
	}
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glfwSetFramebufferSizeCallback(_windowData, resize_hook);
	glfwSetCharCallback(_windowData, keyboard_input);
	glfwSetKeyCallback(_windowData, key_hook);
	glfwSetInputMode(this->getWindowData(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(this->getWindowData(), move_mouse_hook);

	glClearColor(0.4, 0.4, 1.0, 1.0f);
	glEnable(GL_DEPTH_TEST);
	this->center();
	this->setIcon("textures/mbatty.bmp");
}

Window::~Window()
{
	if (DEBUG)
		std::cout << "Destroying window: " << WIN_NAME << std::endl;
	glfwDestroyWindow(this->_windowData);
	glfwTerminate();
}

GLFWwindow	*Window::getWindowData(void)
{
	return (this->_windowData);
}

std::string Window::displayFPS(Font &font, Shader &textShader)
{
	(void)font;(void)textShader;
	std::stringstream	strs;
	strs << (int)(1.0f / _deltaTime) << " fps";

	std::string	str = strs.str();
	return (str);
}

void		Window::loopStart(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_currentFrame = glfwGetTime();
	_deltaTime = _currentFrame - _lastFrame;
}

void		Window::loopEnd()
{
	glfwSwapBuffers(_windowData);
	glfwPollEvents();

	_lastFrame = _currentFrame;
}

bool	Window::up(void)
{
	return (!glfwWindowShouldClose(_windowData));
}

void		Window::setIcon(const char *path)
{
	Texture	tex(path);
	GLFWimage	image[1];
	image[0].pixels = tex.data.data();
	image[0].width = tex.width;
	image[0].height = tex.height;
	glfwSetWindowIcon(_windowData, 1, image);
}

void		Window::center()
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwSetWindowPos(_windowData, (mode->width / 2) - (SCREEN_WIDTH / 2), (mode->height / 2) - (SCREEN_HEIGHT / 2));
}

float		Window::getDeltaTime(void)
{
	return (this->_deltaTime);
}
