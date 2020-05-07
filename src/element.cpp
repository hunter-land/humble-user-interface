#include "./element.hpp"
#include "./set.hpp"

#include <SDL.h>
#include <vector>

using namespace hui;

static const SDL_Point hui::zeroPoint{ 0, 0 };
static const SDL_FPoint hui::zeroFPoint{ 0.f, 0.f };



element::element() {}
element::element(const SDL_FRect dstrect, const double angle, const SDL_RendererFlip flip) {
	setDstrect(dstrect);
	setAngle(angle);
	setFlip(flip);
}
element::~element() {}

void element::render(SDL_Renderer *renderer) {
	//Uint8 r, g, b, a;
	//SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
	//if (m_hasFocus) {
	//	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	//} else {
	//	SDL_SetRenderDrawColor(renderer, 127, 127, 0, 128);
	//}
	//SDL_RenderDrawRect(renderer, &m_dstrect);
	//SDL_SetRenderDrawColor(renderer, r, g, b, a);
}
void element::userLogic(std::vector<SDL_Event> &events, SDL_Renderer *renderer) {
	for (SDL_Event &e : events) {
		if (m_hasFocus) {
			switch (e.type) {
			case SDL_KEYDOWN:
				m_callEventFunction(KeyDown, &e);
				break;
			case SDL_KEYUP:
				m_callEventFunction(KeyUp, &e);
				break;
			case SDL_MOUSEWHEEL:
				m_callEventFunction(MouseWheel, &e);
				break;
			case SDL_MOUSEMOTION:
				m_callEventFunction(MouseMotion, &e);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (e.button.button == SDL_BUTTON_LEFT) {
					m_callEventFunction(LMBDown);
				} else if (e.button.button == SDL_BUTTON_RIGHT) {
					m_callEventFunction(RMBDown);
				} else if (e.button.button == SDL_BUTTON_MIDDLE) {
					m_callEventFunction(MMBDown);
				} else if (e.button.button == SDL_BUTTON_X1) {
					m_callEventFunction(X1BDown);
				} else if (e.button.button == SDL_BUTTON_X2) {
					m_callEventFunction(X2BDown);
				}
				m_callEventFunction(MouseDown, &e);
				break;
			case SDL_MOUSEBUTTONUP:
				if (e.button.button == SDL_BUTTON_LEFT) {
					m_callEventFunction(LMBUp);
				} else if (e.button.button == SDL_BUTTON_RIGHT) {
					m_callEventFunction(RMBUp);
				} else if (e.button.button == SDL_BUTTON_MIDDLE) {
					m_callEventFunction(MMBUp);
				} else if (e.button.button == SDL_BUTTON_X1) {
					m_callEventFunction(X1BUp);
				} else if (e.button.button == SDL_BUTTON_X2) {
					m_callEventFunction(X2BUp);
				}
				m_callEventFunction(MouseUp, &e);
				break;
			}
		}
	}
}
void element::resetUserLogic() {}
void element::loopLogic(Uint32 ms) {}
void element::resetLoopLogic() {}
bool element::bind(Event e, eventFunction func) {
	m_eventFunctions[e] = func;
	return true;
}
bool element::unbind(Event e) {
	m_eventFunctions[e] = nullptr;
	return true;
}

void element::setSet(set* s) {
	if (m_parentSet != nullptr) {
		m_parentSet->removeElement(this);
	}
	m_parentSet = s;
}
void element::setFocus(bool hasFocus) {
	if (hasFocus != m_hasFocus) { //Change detected
		if (hasFocus) {
			//Gained focus
			m_callEventFunction(Event::FocusGained);
		} else {
			//Lost focus
			m_callEventFunction(Event::FocusLost);
		}
		m_hasFocus = hasFocus; //Update member
		onFocusUpdated();
	}
}
void element::onFocusUpdated() {}
void element::setDstrect(SDL_FRect dr) {
	m_dstrect = dr;
	if (m_parentSet != nullptr) {
		m_parentSet->setUpdateFocusElements();
	}
		//DEPRECATED, Now calls to parent set to indicate potential focus change
		//Push a SDL_MOUSEMOTION event so the focus is updated in the next frame
		//SDL_Event ev;
		//ev.type = SDL_MOUSEMOTION;
		//SDL_GetMouseState(&ev.button.x, &ev.button.y);
		//SDL_PushEvent(&ev);
		//End pushing event
}
SDL_FRect element::getDstrect() {
	return m_dstrect;
}
void element::setAngle(double a) {
	m_angle = a;
	if (m_parentSet != nullptr) {
		m_parentSet->setUpdateFocusElements();
	}
		//DEPRECATED, Now calls to parent set to indicate potential focus change
		//Push a SDL_MOUSEMOTION event so the focus is updated in the next frame
		//SDL_Event ev;
		//ev.type = SDL_MOUSEMOTION;
		//SDL_GetMouseState(&ev.button.x, &ev.button.y);
		//SDL_PushEvent(&ev);
		//End pushing event
}
double element::getAngle() {
	return m_angle;
}
void element::setFlip(SDL_RendererFlip flip) {
	m_flip = flip;
}
SDL_RendererFlip element::getFlip() {
	return m_flip;
}

bool element::pointInElement(const SDL_Point point) {
	SDL_Rect temp{ (int)std::round(m_dstrect.x), (int)std::round(m_dstrect.y), (int)std::round(m_dstrect.w), (int)std::round(m_dstrect.h) };
	SDL_Point translatedPoint;
	translatedPoint.x = (int)std::round(std::cos(-m_angle * M_PI / 180) * (point.x - m_dstrect.x) - std::sin(-m_angle * M_PI / 180) * (point.y - m_dstrect.y) + m_dstrect.x);
	translatedPoint.y = (int)std::round(std::sin(-m_angle * M_PI / 180) * (point.x - m_dstrect.x) + std::cos(-m_angle * M_PI / 180) * (point.y - m_dstrect.y) + m_dstrect.y);
	return SDL_PointInRect(&translatedPoint, &temp);
}