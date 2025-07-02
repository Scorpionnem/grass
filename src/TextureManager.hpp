/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TextureManager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 12:22:51 by mbatty            #+#    #+#             */
/*   Updated: 2025/06/08 23:16:22 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEXTUREMANAGER_HPP
# define TEXTUREMANAGER_HPP

#include "libs.hpp"
#include "Texture.hpp"

class	TextureManager
{
	public:
		TextureManager(){}
		~TextureManager()
		{
			for (auto it = loadedTextures.begin(); it != loadedTextures.end(); it++)
				delete it->second;	
		}

		Texture	*load(const std::string &path)
		{
			return (loadedTextures.insert({path, new Texture(path.c_str())}).first->second);
		}
		Texture	*get(const std::string &path)
		{
			std::map<std::string, Texture*>::iterator	finder;
			finder = loadedTextures.find(path);
			if (finder == loadedTextures.end())
				return (load(path));
			return (finder->second);
		}

		std::map<std::string, Texture*>	loadedTextures;
};

extern TextureManager	*TEXTURE_MANAGER;

#endif
