/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Window.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 12:11:18 by mbatty            #+#    #+#             */
/*   Updated: 2025/06/10 22:32:29 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WINDOW_HPP
# define WINDOW_HPP

# include "libs.hpp"
# include "Shader.hpp"
# include "Font.hpp"

extern float	SCREEN_WIDTH;
extern float	SCREEN_HEIGHT;

class	Window
{
	public:
		Window();
		~Window();
		bool	up(void);
		GLFWwindow	*getWindowData(void);
		float		getDeltaTime(void);
		void		loopStart(void);
		void		loopEnd();
		void		setIcon(const char *path);
		void		center();
		float		_deltaTime;
		float		_lastFrame;
		float		_currentFrame;
		std::string displayFPS(Font &font, Shader &textShader);
	private:
		GLFWwindow	*_windowData;
};

#endif
