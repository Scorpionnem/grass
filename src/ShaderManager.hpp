/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 12:17:41 by mbatty            #+#    #+#             */
/*   Updated: 2025/06/10 22:37:53 by mbatty           ###   ########.fr       */
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

		Shader	*get(std::string name)
		{
			return (shaders.find(name)->second);
		}
		void	load(ShaderInfos infos)
		{
			shaders.insert(std::make_pair(infos.name, new Shader(infos.vertex, infos.fragment)));
		}
	private:
		std::map<std::string, Shader *>	shaders;
};

extern	ShaderManager	*SHADER_MANAGER;

#endif
