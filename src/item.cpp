/*#include "./item.hpp"
#include "./itemHolder.hpp"
#include "./set.hpp"

using namespace hui;

template<typename T>
item<T>::item(SDL_Texture *texture, T val) {
	setTexture(texture);
	value = val;
}
template<typename T>
item<T>::item(SDL_Texture *texture, const SDL_FRect dstrect, const double angle, T val) {
	setDstrect(dstrect);
	setAngle(angle);
	setTexture(texture);
	value = val;
}
template<typename T>
item<T>::~item() {}

template<typename T>
void item<>::render(SDL_Renderer *renderer) {
	SDL_RenderCopyExF(renderer, m_texture, srcrect, &m_dstrect, m_angle, &(hui::zeroPointF), SDL_FLIP_NONE);
}
template<typename T>
void item<T>::userLogic(std::vector<SDL_Event> &events, SDL_Renderer *renderer) {
	element::userLogic(events, renderer);
	for (SDL_Event &e : events) {
		if (m_hasFocus) {
			switch (e.type) {
			case SDL_MOUSEMOTION:
				if (m_beingMoved) {
					m_dstrect.x += e.motion.xrel;
					m_dstrect.y += e.motion.yrel;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (e.button.button == SDL_BUTTON_LEFT) {
					//Start dragging item.
					m_beingMoved = true;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (e.button.button == SDL_BUTTON_LEFT) {
					//Release item.
					m_beingMoved = false;
					//Check if we were released on an itemHolder
					if (m_parentSet != nullptr) {
						std::vector<element*> focused = m_parentSet->getFocusedElements();
						itemHolder<T>* focusedSub = dynamic_cast<itemHolder<T>*>(focused[1]);
						if (focused[0] == this && focusedSub != nullptr) {
							focusedSub->setChild(this);
						}
					}
				}
				break;
			}
		}
	}
}

template<typename T>
void item<T>::setTexture(SDL_Texture *texture) {
	m_texture = texture;
}
template<typename T>
SDL_Texture* item<T>::getTexture() {
	return m_texture;
}
*/