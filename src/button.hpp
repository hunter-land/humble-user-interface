#pragma once

#include "./element.hpp"

#include <SDL.h>
#include <vector>

namespace hui {
	/**
	 *	\brief A basic button
	 */
	class button : public element {
	protected:
		SDL_Texture *m_texture = nullptr; //!<The texture of the button
	public:
		SDL_Rect *srcrect = nullptr; //!<Rectangle of the texture to draw

		/**
		 *	\brief Construct button with a texture
		 *
		 *	\param *texture The texture of the button.
		 */
		button(SDL_Texture *texture);
		/**
		 *	\brief Construct button with a texture, position, size, and angle
		 *
		 *	\param *texture The texture of the button.
		 *	\param dstrect The rectangle to draw the button in.
		 *	\param angle The angle the button will be at.
		 *	\param flip The flip the button will have.
		 */
		button(SDL_Texture *texture, const SDL_FRect dstrect, const double angle = 0, const SDL_RendererFlip flip = SDL_FLIP_NONE);
		/**
		 *	\brief Deconstruct button
		 */
		~button();

		void render(SDL_Renderer *renderer);
		void userLogic(std::vector<SDL_Event> &events, SDL_Renderer *renderer);
		using element::resetUserLogic;
		using element::loopLogic;
		using element::resetLoopLogic;
		using element::bind;
		using element::unbind;

		using element::setFocus;
		using element::setDstrect;
		using element::getDstrect;
		using element::setAngle;
		using element::getAngle;
		/**
		 *	\brief Sets the button's texture
		 *
		 *	\param *texture The texture for the button to use
		 */
		void setTexture(SDL_Texture *texture);
		/**
		 *	\brief Gets the button's texture
		 *
		 *	\returns The texture of the button
		 */
		SDL_Texture* getTexture();

		using element::pointInElement;
	};
};