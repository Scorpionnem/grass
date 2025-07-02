/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EmittersManager.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 11:53:53 by mbatty            #+#    #+#             */
/*   Updated: 2025/06/25 11:56:28 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EMITTERSMANAGER_HPP
# define EMITTERSMANAGER_HPP

#include "libs.hpp"
#include "Emitter.hpp"
#include "Window.hpp"

extern bool		PAUSED;
extern bool		GRAVITY_CENTER_ACTIVATED;
extern Window	*WINDOW;

class	EmittersManager
{
	public:
		EmittersManager()
		{
		}
		~EmittersManager()
		{
			for (auto *emitter : emitters)
				delete emitter;
		}
		void	addEmitter()
		{
			if (emitters.size() < 16)
				emitters.push_back(new Emitter(CAMERA->pos));
		}
		void	render()
		{
			for (auto *emitter : emitters)
				emitter->render();
		}
		void	update(glm::vec3 attractor, glm::vec3 secondaryAttractor)
		{
			for (auto *emitter : emitters)
				emitter->update(PAUSED, GRAVITY_CENTER_ACTIVATED, attractor, secondaryAttractor, WINDOW->getDeltaTime());
		}
		void	compact()
		{
			for (auto *emitter : emitters)
				emitter->compact();
		}
	private:
		std::vector<Emitter *>	emitters;
};

#endif
