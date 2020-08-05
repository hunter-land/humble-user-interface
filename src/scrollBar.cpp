#include "./scrollBar.hpp"

extern "C" {
#include <SDL2/SDL.h>
}
#include "./extra.hpp" //RotatePoint

namespace lui {

	scrollBar::scrollBar(SDL_Texture *trackTexture, SDL_Texture *gripTexture) {
		setTrackTexture(trackTexture);
		setGripTexture(gripTexture);
	}
	scrollBar::scrollBar(SDL_Texture *trackTexture, SDL_Texture *gripTexture, const SDL_FRect dstrectTrack, const SDL_FRect dstrectGrip, const double angle) {
		setTrackTexture(trackTexture);
		setGripTexture(gripTexture);
		setDstrect(dstrectTrack);
		setGripDstrect(dstrectGrip);
		setAngle(angle);
	}
	scrollBar::~scrollBar() {}

	void scrollBar::render(SDL_Renderer *renderer) {
		SDL_RenderCopyExF(renderer, m_textureTrack, srcrectTrack, &m_dstrect, m_angle, &zeroFPoint, m_flip);
		SDL_RenderCopyExF(renderer, m_textureGrip, srcrectGrip, &m_dstrectGrip, m_angle, &zeroFPoint, m_flip);
	}
	void scrollBar::userLogic(std::vector<SDL_Event> &events, SDL_Renderer *renderer) {
		for (SDL_Event &e : events) {
			if (m_hasFocus) {
				switch (e.type) {
				case SDL_KEYDOWN:
					if (m_gripHasFocus) {
						m_callGripEventFunction(KeyDown, &e);
					}
					else {
						m_callEventFunction(KeyDown, &e);
					}
					break;
				case SDL_KEYUP:
					if (m_gripHasFocus) {
						m_callGripEventFunction(KeyUp, &e);
					}
					else {
						m_callEventFunction(KeyUp, &e);
					}
					break;
				case SDL_MOUSEWHEEL:
					if (m_gripHasFocus) {
						m_callGripEventFunction(MouseWheel, &e);
					}
					else {
						m_callEventFunction(MouseWheel, &e);
					}
					break;
				case SDL_MOUSEMOTION:
					//Check if grip still has focus
				{
					m_updateGripFocus = false;
					bool gripNewFocus = pointInGrip({ e.button.x, e.button.y });
					if (m_gripHasFocus != gripNewFocus) {
						if (m_gripHasFocus && !gripNewFocus) {
							m_callGripEventFunction(FocusLost, &e);
						}
						else {
							m_callGripEventFunction(FocusGained, &e);
						}
						m_gripHasFocus = gripNewFocus;
					}
				}
				if (m_gripHasFocus) {
					m_callGripEventFunction(MouseMotion, &e);
				}
				else {
					m_callEventFunction(MouseMotion, &e);
				}
				break;
				case SDL_MOUSEBUTTONDOWN:
					if (e.button.button == SDL_BUTTON_LEFT) {
						if (m_gripHasFocus) {
							//Start dragging grip.
							m_gripMoving = true;
							m_callGripEventFunction(Event::LMBDown, &e);
						}
						else {
							//Jump in the direction of the press (Jump distance is equal to the size of the grip)
							//Get the vector the click was in, relative to the track and grip
							SDL_FPoint clickLocal = RotatePoint({ (float)e.button.x, (float)e.button.y }, -m_angle, { m_dstrect.x, m_dstrect.y });
							clickLocal.x -= m_dstrect.x;
							clickLocal.y -= m_dstrect.y;
							SDL_FPoint gripLocalCenter{ m_gripPos.x * (m_dstrect.w - m_dstrectGrip.w) + m_dstrectGrip.w / 2,
														m_gripPos.y * (m_dstrect.h - m_dstrectGrip.h) + m_dstrectGrip.h / 2 };
							SDL_FPoint clickUnitVector{ 0, 0 };
							if (m_dstrectGrip.w < m_dstrect.w) {
								clickUnitVector.x = clickLocal.x - gripLocalCenter.x;
							}
							else if (m_dstrectGrip.h < m_dstrect.h) {
								clickUnitVector.y = clickLocal.y - gripLocalCenter.y;
							}
							double clickVectorLength = std::pow(std::pow(clickUnitVector.x, 2) + std::pow(clickUnitVector.y, 2), 0.5);
							clickUnitVector.x /= clickVectorLength;
							clickUnitVector.y /= clickVectorLength;
							//Multiply that unit vector by the grip size (relative axis) m_dstrect.w / (m_dstrect.w - m_dstrectGrip.w)
							SDL_FPoint movementVector{ clickUnitVector.x * m_dstrectGrip.w / (m_dstrect.w - m_dstrectGrip.w),
														clickUnitVector.y * m_dstrectGrip.h / (m_dstrect.h - m_dstrectGrip.h) };
							//Move by this amount (add previous position to it)
							setGripPosition({ m_gripPos.x + movementVector.x, m_gripPos.y + movementVector.y });
						}
						if (m_gripHasFocus) {
							m_callGripEventFunction(LMBDown);
						}
						else {
							m_callEventFunction(LMBDown);
						}
					}
					else if (e.button.button == SDL_BUTTON_RIGHT) {
						if (m_gripHasFocus) {
							m_callGripEventFunction(RMBDown);
						}
						else {
							m_callEventFunction(RMBDown);
						}
					}
					else if (e.button.button == SDL_BUTTON_MIDDLE) {
						if (m_gripHasFocus) {
							m_callGripEventFunction(MMBDown);
						}
						else {
							m_callEventFunction(MMBDown);
						}
					}
					else if (e.button.button == SDL_BUTTON_X1) {
						if (m_gripHasFocus) {
							m_callGripEventFunction(X1BDown);
						}
						else {
							m_callEventFunction(X1BDown);
						}
					}
					else if (e.button.button == SDL_BUTTON_X2) {
						if (m_gripHasFocus) {
							m_callGripEventFunction(X2BDown);
						}
						else {
							m_callEventFunction(X2BDown);
						}
					}
					if (m_gripHasFocus) {
						m_callGripEventFunction(MouseDown, &e);
					}
					else {
						m_callEventFunction(MouseDown, &e);
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if (e.button.button == SDL_BUTTON_LEFT) {
						if (m_gripHasFocus) {
							m_callGripEventFunction(LMBUp);
						}
						else {
							m_callEventFunction(LMBUp);
						}
					}
					else if (e.button.button == SDL_BUTTON_RIGHT) {
						if (m_gripHasFocus) {
							m_callGripEventFunction(RMBUp);
						}
						else {
							m_callEventFunction(RMBUp);
						}
					}
					else if (e.button.button == SDL_BUTTON_MIDDLE) {
						if (m_gripHasFocus) {
							m_callGripEventFunction(MMBUp);
						}
						else {
							m_callEventFunction(MMBUp);
						}
					}
					else if (e.button.button == SDL_BUTTON_X1) {
						if (m_gripHasFocus) {
							m_callGripEventFunction(X1BUp);
						}
						else {
							m_callEventFunction(X1BUp);
						}
					}
					else if (e.button.button == SDL_BUTTON_X2) {
						if (m_gripHasFocus) {
							m_callGripEventFunction(X2BUp);
						}
						else {
							m_callEventFunction(X2BUp);
						}
					}
					if (m_gripHasFocus) {
						m_callGripEventFunction(MouseUp, &e);
					}
					else {
						m_callEventFunction(MouseUp, &e);
					}
					break;
				}
			}
			//Check for grip release even after we loose focus
			if (m_gripMoving && e.type == SDL_MOUSEBUTTONUP) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					m_gripMoving = false;
				}
			}
			//Move grip, keep in track.
			if (m_gripMoving && e.type == SDL_MOUSEMOTION) {
				//Translate motion.(x/y)rel to account for our angle
				SDL_FPoint xyrel = RotatePoint({ (float)e.motion.xrel, (float)e.motion.yrel }, -m_angle);

				setGripPosition({ m_gripPos.x + (xyrel.x / (m_dstrect.w - m_dstrectGrip.w)), m_gripPos.y + (xyrel.y / (m_dstrect.h - m_dstrectGrip.h)) });
			}
			//Update m_gripHasFocus
			if (m_updateGripFocus) {
				m_updateGripFocus = false;
				bool gripNewFocus = pointInGrip({ e.button.x, e.button.y });
				if (m_gripHasFocus != gripNewFocus) {
					if (m_gripHasFocus && !gripNewFocus) {
						m_callGripEventFunction(FocusLost, &e);
					}
					else {
						m_callGripEventFunction(FocusGained, &e);
					}
					m_gripHasFocus = gripNewFocus;
				}
			}
		}
	}
	void scrollBar::resetUserLogic() {
		setGripPosition({ 0, 0 });
		m_gripMoving = false;
	}
	bool scrollBar::bind(Event e, eventFunction func, Component c) {
		switch (c) {
		case Component::Grip:
			m_gripEventFunctions[e] = func;
			break;
		case Component::Track:
			m_eventFunctions[e] = func;
			break;
		default:
			return false;
			break;
		}
		return true;
	}
	bool scrollBar::unbind(Event e, Component c) {
		switch (c) {
		case Component::Grip:
			m_gripEventFunctions[e] = nullptr;
			break;
		case Component::Track:
			m_eventFunctions[e] = nullptr;
			break;
		default:
			return false;
			break;
		}
		return true;
	}

	void scrollBar::onFocusUpdated() {
		//if (!m_hasFocus) {
		//	m_gripMoving = false;
		//}
	}
	void scrollBar::setDstrect(SDL_FRect dr) {
		element::setDstrect(dr);
		m_dstrectGrip.w = std::min(m_dstrectGrip.w, m_dstrect.w);
		m_dstrectGrip.h = std::min(m_dstrectGrip.h, m_dstrect.h);
	}
	void scrollBar::setAngle(double a) {
		element::setAngle(a);
		setGripPosition(m_gripPos);
	}
	void scrollBar::setTrackTexture(SDL_Texture *texture) {
		m_textureTrack = texture;
	}
	SDL_Texture* scrollBar::getTrackTexture() {
		return m_textureTrack;
	}
	void scrollBar::setGripTexture(SDL_Texture *texture) {
		m_textureGrip = texture;
	}
	SDL_Texture* scrollBar::getGripTexture() {
		return m_textureGrip;
	}
	void scrollBar::setGripDstrect(SDL_FRect grprect) {
		grprect.w = std::min(grprect.w, m_dstrect.w);
		grprect.h = std::min(grprect.h, m_dstrect.h);
		m_dstrectGrip = grprect;
		setGripPosition({ m_dstrectGrip.x, m_dstrectGrip.y });
	}
	SDL_FRect scrollBar::getGripDstrect() {
		return m_dstrectGrip;
	}
	void scrollBar::setGripPosition(SDL_FPoint pos) {
		//Set position and update attached scrollWindows (if any)
		pos.x = std::max(0.f, std::min(pos.x, 1.f));
		pos.y = std::max(0.f, std::min(pos.y, 1.f));
		m_gripPos = pos;
		SDL_FPoint gripPos = RotatePoint({ m_gripPos.x * (m_dstrect.w - m_dstrectGrip.w), m_gripPos.y * (m_dstrect.h - m_dstrectGrip.h) }, m_angle);
		m_dstrectGrip.x = gripPos.x + m_dstrect.x;
		m_dstrectGrip.y = gripPos.y + m_dstrect.y;
		m_callGripEventFunction(Event::ValueChanged);
		m_updateGripFocus = true;
	}
	SDL_FPoint scrollBar::getGripPosition() {
		return m_gripPos;
	}
	//DEPREACTED:
	/*
	bool scrollBar::attachScrollWindow(scrollWindow *sw) {
		if (std::find(m_attachedScrollWindows.begin(), m_attachedScrollWindows.end(), sw) == m_attachedScrollWindows.end()) { //No duplicates allowed
			m_attachedScrollWindows.push_back(sw);
			//sw->setWindowPosition(m_gripPos);
			return true;
		}
		return false;
	}
	bool scrollBar::removeScrollWindow(scrollWindow *sw) {
		std::vector<scrollWindow*>::iterator swPos = std::find(m_attachedScrollWindows.begin(), m_attachedScrollWindows.end(), sw);
		if (swPos != m_attachedScrollWindows.end()) {
			m_attachedScrollWindows.erase(swPos);
			return true;
		}
		return false;
	}
	scrollWindow* scrollBar::removeScrollWindow(size_t position) {
		if (m_attachedScrollWindows.size() > position) {
			scrollWindow *detachedWindow = m_attachedScrollWindows[position];
			m_attachedScrollWindows.erase(m_attachedScrollWindows.begin() + position);
			return detachedWindow;
		}
		return nullptr;
	}
	std::vector<scrollWindow*> scrollBar::getAttachedScrollWindows() {
		return m_attachedScrollWindows;
	}//*/

	bool scrollBar::pointInGrip(SDL_Point point) {
		SDL_Rect temp{ (int)std::round(m_dstrect.x + (m_dstrect.w - m_dstrectGrip.w) * m_gripPos.x),
						(int)std::round(m_dstrect.y + (m_dstrect.h - m_dstrectGrip.h) * m_gripPos.y),
						(int)std::round(m_dstrectGrip.w),
						(int)std::round(m_dstrectGrip.h) };

		SDL_FPoint rotatedPoint = RotatePoint({ (float)point.x, (float)point.y }, -m_angle, { m_dstrect.x, m_dstrect.y });
		SDL_Point translatedPoint{ (int)std::round(rotatedPoint.x), (int)std::round(rotatedPoint.y) };
		return SDL_PointInRect(&translatedPoint, &temp);
	}

}
