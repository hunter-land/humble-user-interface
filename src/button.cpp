#include "./button.hpp"

extern "C" {
#include <SDL2/SDL.h>
}

namespace lui {

	button::button(SDL_Texture *texture) {
		setTexture(texture);
	}
	button::button(SDL_Texture *texture, const SDL_FRect dstrect, const double angle, const SDL_RendererFlip flip) {
		setDstrect(dstrect);
		setAngle(angle);
		setFlip(flip);
		setTexture(texture);
	}
	button::~button() {}

	void button::render(SDL_Renderer *renderer) {
		SDL_RenderCopyExF(renderer, m_texture, srcrect, &m_dstrect, m_angle, &(zeroFPoint), m_flip);
	}
	void button::userLogic(std::vector<SDL_Event> &events, SDL_Renderer *renderer) {
		element::userLogic(events, renderer);
	}

	void button::setTexture(SDL_Texture *texture) {
		m_texture = texture;
	}
	SDL_Texture* button::getTexture() {
		return m_texture;
	}

}
