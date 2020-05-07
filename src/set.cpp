#include "./set.hpp"
#include "./element.hpp"
#include "./item.hpp"
#include "./itemHolder.hpp"

using namespace hui;

set::set(std::vector<element*> elements) {
	m_hasFocus = true;
	for (element* e : elements) {
		addElement(e);
	}
}
set::~set() {
	SDL_DestroyTexture(m_texture);
}

void set::render(SDL_Renderer *renderer) {
	SDL_Texture *prevRenderTarget = nullptr;
	//If dstrect size > 0
	//	Lock renderer mutex
	//	If no texture
	//		Create texture
	//	Set renderer target to our texture
	//Else
	//	If texture
	//		Remove texture
	if (m_dstrect.w > 0 && m_dstrect.h > 0) {
		m_textureMutex.lock();
		if (m_texture == nullptr) {
			m_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, m_dstrect.w, m_dstrect.h);
			SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
		}
		prevRenderTarget = SDL_GetRenderTarget(renderer);
		SDL_SetRenderTarget(renderer, m_texture);
		Uint8 r, g, b, a;
		SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
		SDL_SetRenderDrawColor(renderer, m_r, m_g, m_b, m_a);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, r, g, b, a);
	} else if (m_texture != nullptr && m_dstrect.w <= 0 && m_dstrect.h <= 0) {
		SDL_DestroyTexture(m_texture);
		m_texture = nullptr;
	}
	//Render to target
	for (element* &e : m_elements) {
		SDL_FRect prevDst = e->getDstrect(), tempDst = prevDst;
		tempDst.x += m_renderCorner.x;
		tempDst.y += m_renderCorner.y;
		e->setDstrect(tempDst); //TODO: Remove this line and find an efficient way to translate children; This line causes focus re-evaluation.
		e->render(renderer);
		e->setDstrect(prevDst); //TODO: Remove this line and find an efficient way to translate children; This line causes focus re-evaluation.
	}
	//If dstrect size > 0
	//	Set renderer target back to previous
	//	Draw our texture in desired location
	//	Free mutex
	if (m_dstrect.w > 0 && m_dstrect.h > 0) {
		SDL_SetRenderTarget(renderer, prevRenderTarget);
		m_textureMutex.unlock();
		SDL_RenderCopyExF(renderer, m_texture, NULL, &m_dstrect, m_angle, &zeroFPoint, m_flip);
	}
}
void set::userLogic(std::vector<SDL_Event> &events, SDL_Renderer *renderer) {
	if (m_hasFocus) {
		m_elementsMutex.lock();

		unsigned char m_cursorDepth = 0;
		bool updateFocusElements = m_updateFocusElements;
		m_updateFocusElements = false;
		SDL_Point mousePoint = { 0, 0 };

		std::vector<SDL_Event> translatedEvents;
		for (SDL_Event &e : events) {
			if (e.type == SDL_MOUSEMOTION) {
				updateFocusElements = true;
				mousePoint.x = e.button.x;
				mousePoint.y = e.button.y;
			}
			//TODO: Make sure this coveres all relavent points
			//Rotate all points around m_dstrect(x,y) by -m_angle
			//Then subtract m_dstrect(x,y) to get translated point
			if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEMOTION) {
				SDL_FPoint translatedPoint = RotatePoint({ (float)e.button.x, (float)e.button.y }, -m_angle, { m_dstrect.x, m_dstrect.y });
				e.button.x = translatedPoint.x - m_dstrect.x - m_renderCorner.x;
				e.button.y = translatedPoint.y - m_dstrect.y - m_renderCorner.y;
			}
			//TODO: Check and make sure this is getting the same end value as windowToLocal(mousePoint) is for applicable actions.
			//	//REASON: When moving cursor over a sub-enough element, focus is lost and gained in 1-2 frames, when there should be no change
			translatedEvents.push_back(e);
		}
		if (mousePoint.x == 0 && mousePoint.y == 0) {
			SDL_GetMouseState(&mousePoint.x, &mousePoint.y);
		}

		SDL_FPoint mousePointF = windowToLocal(SDL_FPoint{ (float)mousePoint.x, (float)mousePoint.y });
		mousePoint.x = std::round(mousePointF.x);
		mousePoint.y = std::round(mousePointF.y);

		for (int i = m_elements.size() - 1; i >= 0; i--) {
			//Run the logic for the element
			(m_elements[i])->userLogic(translatedEvents, renderer);

			//If we are updating elements AND there is room for more, find check if this element is the next in line
			if (updateFocusElements && m_cursorDepth < m_focusedElements.size()) {
				//If the cursor is on the element, add it
				if ((m_elements[i])->pointInElement(mousePoint)) {
					//If it is the top most element, let it know
					if (m_cursorDepth == 0 && m_focusedElements[m_cursorDepth] != (m_elements[i])) {
						if (m_focusedElements[m_cursorDepth] != nullptr) {
							m_focusedElements[m_cursorDepth]->setFocus(false);
						}
						m_focusedElements[m_cursorDepth] = (m_elements[i]);
						m_focusedElements[m_cursorDepth]->setFocus(true);
					}
					else {
						m_focusedElements[m_cursorDepth] = (m_elements[i]); //Not the top most, so it doesn't know, change it.
					}
					//Increment element depth for this update
					m_cursorDepth++;
				}
			}
		}

		//If we are updating focus, set the ones not updated to nullptr
		if (updateFocusElements) {
			for (size_t i = m_cursorDepth; i < m_focusedElements.size(); i++) {
				if (m_focusedElements[i] != nullptr) {
					if (i == 0) { //Since the top element is active, let it know it has lost focus
						m_focusedElements[i]->setFocus(false);
					}
					m_focusedElements[i] = nullptr;
				}
			}
		}

		m_elementsMutex.unlock();
	}
}
void set::resetUserLogic() {
	for (element* &e : m_elements) {
		e->resetUserLogic();
	}
}
void set::loopLogic(Uint32 ms) {
	for (element* &e : m_elements) {
		e->loopLogic(ms);
	}
}
void set::resetLoopLogic() {
	for (element* &e : m_elements) {
		e->resetLoopLogic();
	}
}

void set::setSet(set* s) {
	element::setSet(s);
	//setFocus(m_parentSet == nullptr);
}
void set::onFocusUpdated() {
	if (m_hasFocus) {
		setUpdateFocusElements();
	} else {
		for (size_t i = 0; i < m_focusedElements.size(); i++) {
			if (m_focusedElements[i] != nullptr) {
				if (i == 0) { //Since the top element is active, let it know it has lost focus
					m_focusedElements[i]->setFocus(false);
				}
				m_focusedElements[i] = nullptr;
			}
		}
	}
}
void set::setDstrect(SDL_FRect dr) {
	if (m_texture != nullptr && (dr.w != m_dstrect.w || dr.h != m_dstrect.h)) {
		SDL_DestroyTexture(m_texture);
		m_texture = nullptr;
		//No texture created here because we don't have the render target, and it will be automatically created (if needed) in render()
	}
	element::setDstrect(dr);
}
void set::setRenderCorner(SDL_FPoint shift) {
	m_renderCorner = shift;
	setUpdateFocusElements();
}
SDL_FPoint set::getRenderCorner() {
	return m_renderCorner;
}
bool set::addElement(element *e) {
	if (e == this) { //Cannot add self as child
		return false;
	}
	m_elementsMutex.lock();
	if (std::find(m_elements.begin(), m_elements.end(), e) == m_elements.end()) { //No duplicates allowed
		m_elements.push_back(e);
		e->setSet(this);
		e->setFocus(false);
		setUpdateFocusElements();
		m_elementsMutex.unlock();
		return true;
	} else {
		m_elementsMutex.unlock();
		return false;
	}
}
size_t set::addElement(std::vector<element*> es) {
	size_t added = 0;
	for (element* e : es) {
		added += addElement(e);
	}
	return added;
}
bool set::removeElement(element *e) {
	m_elementsMutex.lock();
	size_t elementsBefore = m_elements.size();
	for (auto it = m_elements.begin(); it != m_elements.end(); it++) { //Find the element we want to remove
		if (*it == e) { //If this is the one to remove
			e->setSet(nullptr);
			m_elements.erase(it); //Remove it
			break;
		}
	}
	m_elementsMutex.unlock();
	return (elementsBefore < m_elements.size());
}
element* set::removeElement(size_t position) {
	m_elementsMutex.lock();
	element* removedElement = nullptr;
	if (position < m_elements.size()) {
		removedElement = m_elements[position];
		m_elements[position]->setSet(nullptr);
		m_elements.erase(m_elements.begin() + position);
	}
	m_elementsMutex.unlock();
	return removedElement;
}
std::array<element*, 2> set::getFocusedElements() {
	m_elementsMutex.lock();
	std::array<element*, 2> focusedElements = m_focusedElements;
	m_elementsMutex.unlock();
	return focusedElements;
}
void set::setUpdateFocusElements() {
	m_updateFocusElements = true;
}

SDL_Point set::windowToLocal(SDL_Point windowPoint) {
	if (m_parentSet != nullptr) {
		windowPoint = m_parentSet->windowToLocal(windowPoint);
	}
	SDL_FPoint windowPointRotated = RotatePoint({ (float)windowPoint.x, (float)windowPoint.y }, -m_angle, { m_dstrect.x, m_dstrect.y });
	windowPoint.x = windowPointRotated.x - m_dstrect.x - m_renderCorner.x;
	windowPoint.y = windowPointRotated.y - m_dstrect.y - m_renderCorner.y;
	
	return windowPoint;
}
SDL_FPoint set::windowToLocal(SDL_FPoint windowPoint) {
	if (m_parentSet != nullptr) {
		windowPoint = m_parentSet->windowToLocal(windowPoint);
	}
	windowPoint = RotatePoint({ windowPoint.x, windowPoint.y }, -m_angle, { m_dstrect.x, m_dstrect.y });
	windowPoint.x += - m_dstrect.x - m_renderCorner.x;
	windowPoint.y += - m_dstrect.y - m_renderCorner.y;

	return windowPoint;
}
SDL_Point set::localToWindow(SDL_Point localPoint) {
	localPoint.x += m_dstrect.x + m_renderCorner.x;
	localPoint.y += m_dstrect.y + m_renderCorner.y;
	SDL_FPoint localPointRotated = RotatePoint({ (float)localPoint.x, (float)localPoint.y }, m_angle, { m_dstrect.x, m_dstrect.y });
	if (m_parentSet != nullptr) {
		localPoint = m_parentSet->localToWindow(SDL_Point{ (int)localPointRotated.x, (int)localPointRotated.y });
	}
	
	return localPoint;
}
SDL_FPoint set::localToWindow(SDL_FPoint localPoint) {
	localPoint.x += m_dstrect.x + m_renderCorner.x;
	localPoint.y += m_dstrect.y + m_renderCorner.y;
	SDL_FPoint localPointRotated = RotatePoint({ localPoint.x, localPoint.y }, m_angle, { m_dstrect.x, m_dstrect.y });
	if (m_parentSet != nullptr) {
		localPoint = m_parentSet->localToWindow(SDL_FPoint{ localPointRotated.x, localPointRotated.y });
	}

	return localPoint;
}