/*#include "./itemHolder.hpp"
#include "./item.hpp"

using namespace hui;

template<typename T>
itemHolder<T>::itemHolder(SDL_Texture *texture, bool(*func)(itemHolder<T>*, item<T>*)) {
	setTexture(texture);
	setAcceptFunction(func);
}
template<typename T>
itemHolder<T>::itemHolder(SDL_Texture *texture, const SDL_FRect dstrect, const double angle, bool(*func)(itemHolder<T>*, item<T>*)) {
	setDstrect(dstrect);
	setAngle(angle);
	setTexture(texture);
	setAcceptFunction(func);
}
template<typename T>
itemHolder<T>::~itemHolder() {}

template<typename T>
void itemHolder<T>::render(SDL_Renderer *renderer) {
	SDL_RenderCopyExF(renderer, m_texture, srcrect, &m_dstrect, m_angle, &(hui::zeroPointF), SDL_FLIP_NONE);
}
//template<typename T>
//void itemHolder<T>::userLogic(std::vector<SDL_Event> &events, SDL_Renderer *renderer) {
//	element::userLogic(events, renderer);
//}
template<typename T>
void itemHolder<T>::setAcceptFunction(bool(*func)(itemHolder<T>*, item<T>*)) {
	m_acceptFunction = func;
}

template<typename T>
void itemHolder<T>::setTexture(SDL_Texture *texture) {
	m_texture = texture;
}
template<typename T>
SDL_Texture* itemHolder<T>::getTexture() {
	return m_texture;
}
template<typename T>
template<typename U>
bool itemHolder<T>::setChild(item<U> *child) {
	if (std::is_same<T, U>::value) { //If they are not of same type, they can not (will not) be compared.
		if (m_acceptFunction(this, child) && (m_childItem == nullptr && child != nullptr) || (m_childItem != nullptr && child == nullptr)) {
			m_childItem = child;
			if (child != nullptr) { //No current child, and a valid child
				auto childDstrect = m_childItem->getDstrect();
				childDstrect.x = m_dstrect.x + m_dstrect.w / 2 - childDstrect.w / 2;
				childDstrect.y = m_dstrect.y + m_dstrect.h / 2 - childDstrect.h / 2;
				m_childItem->setDstrect(childDstrect);
			}
			m_callEventFunction(Event::ValueChange);
			return true;
		}
	}
	return false;
}
template<typename T>
item<T> itemHolder<T>::getChild() {
	return m_childItem;
}
*/