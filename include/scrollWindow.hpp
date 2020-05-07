#pragma once

#include "./element.hpp"

#include <SDL.h>

namespace hui {
	class scrollWindow : public element {
	protected:
		SDL_FRect m_cameraRect; //<!Position and size of the camera which views child elements
		SDL_Rect m_viewportRect; //<!Rectangle the camera's view will be rendered in
	public:
		scrollWindow();

		void render(SDL_Renderer *renderer);

		void setWindowPosition(SDL_FPoint position, bool updateScrollBar = false);
	};
};