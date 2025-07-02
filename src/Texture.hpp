/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 13:48:51 by mbatty            #+#    #+#             */
/*   Updated: 2025/05/28 16:41:52 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEXTURE_HPP
# define TEXTURE_HPP

# include "libs.hpp"

class	Texture
{
	public:
		Texture(void)
		{
			this->ID = 0;
		}
		Texture(const char *path);
		Texture(const Texture &cpy)
		{
			this->ID = cpy.ID;
		}
		Texture(const int ID)
		{
			this->ID = ID;
		}
		//Takes ID from cpy (rendering it useless) its pretty much a moove assignor
		void	cut(Texture &cpy);
		void	LoadImage(const char *path);
		~Texture();
		void	use(void);
		void	use1(void);
		void	use2(void);
		static void	reset(void);
		unsigned int	ID;
		std::vector<unsigned char>	data;
		int	width;
		int	height;
		std::string	path;
	private:
};

#endif