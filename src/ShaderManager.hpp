/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 12:17:41 by mbatty            #+#    #+#             */
/*   Updated: 2025/07/09 15:25:56 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADERMANAGER_HPP
# define SHADERMANAGER_HPP

# include "libs.hpp"
# include "Shader.hpp"

struct	ShaderInfos
{
	const std::string	&name;
	const char	*vertex;
	const char	*fragment;
};

class	ShaderManager
{
	public:
		ShaderManager()
		{
		}
		~ShaderManager()
		{
			for (auto it = shaders.begin(); it != shaders.end(); it++)
				delete it->second;
		}

		bool	erase(const std::string &name)
		{
			std::map<std::string, Shader *>::iterator	finder;
			finder = shaders.find(name);
			if (finder == shaders.end())
			{
				consoleLog("WARNING Tried to unload a shader thats not loaded: " + name, LogSeverity::WARNING);
				return (0);
			}
			shaders.erase(finder);
			return (1);
		}
		void	load(ShaderInfos infos)
		{
			shaders.insert(std::make_pair(infos.name, new Shader(infos.vertex, infos.fragment)));
		}
		Shader	*get(const std::string &name)
		{
			std::map<std::string, Shader *>::iterator	finder = shaders.find(name);
			if (finder == shaders.end())
			{
				consoleLog("WARNING Tried to access a shader thats not loaded: " + name, LogSeverity::WARNING);
				return (NULL);
			}
			return (finder->second);
		}
		Shader	*operator[](const std::string &name)
		{
			return (this->get(name));
		}
	private:
		std::map<std::string, Shader *>	shaders;
};

extern	ShaderManager	*SHADER_MANAGER;

#endif
