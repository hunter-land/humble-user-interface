#pragma once

#include "./element.hpp"
#include "./itemHolder.hpp"

#include <SDL.h>
#include <vector>

namespace hui {
	
	/**
	 *	\brief A draggable item which can be placed inside itemHolders
	 *
	 *	\note Item template type must match the itemHolder's template type for the item to be held.
	 */
	template<typename T>
	class item : public element {
	protected:
		SDL_Texture *m_texture = nullptr; //!<The texture of the item
		bool m_beingMoved = false; //!<Is this item currently being moved by the user
	public:
		SDL_Rect *srcrect = nullptr; //!<Rectangle of the texture to draw
		itemHolder<T> *m_holder = nullptr; //!<The itemHolder to which we are assigned \warning This item should never be written to.
		T value; //!<Value, or ID, of this item

		/**
		 *	\brief Construct item with a texture and a value
		 *
		 *	\param *texture The texture of the item.
		 *	\param val The value to assign to the item.
		 */
		item(SDL_Texture *texture, T val = T()) {
			setDstrect(dstrect);
			value = val;
		}
		/**
		 *	\brief Construct item with a texture, position, size, angle, and value
		 *
		 *	\param *texture The texture of the item.
		 *	\param dstrect The rectangle to draw the item in.
		 *	\param angle The angle the item will be at.
		 *	\param flip The flip the item will have.
		 *	\param val The value to assign to the item.
		 */
		item(SDL_Texture *texture, const SDL_FRect dstrect, const double angle = 0, const SDL_RendererFlip flip = SDL_FLIP_NONE, T val = T()) {
			setDstrect(dstrect);
			setAngle(angle);
			setTexture(texture);
			value = val;
		}
		/**
		 *	\brief Deconstruct item
		 */
		~item() {}

		void render(SDL_Renderer *renderer) {
			SDL_RenderCopyExF(renderer, m_texture, srcrect, &m_dstrect, m_angle, &(hui::zeroFPoint), m_flip);
		}
		void userLogic(std::vector<SDL_Event> &events, SDL_Renderer *renderer) {
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
							pickup();
						}
						break;
					case SDL_MOUSEBUTTONUP:
						if (e.button.button == SDL_BUTTON_LEFT) {
							//Release item
							release();
						}
						break;
					}
				}
			}
		}
		void resetUserLogic() {
			element::resetUserLogic();
			release();
		}
		using element::loopLogic;
		using element::resetLoopLogic;
		using element::bind;
		using element::unbind;

		using element::setSet;
		using element::setFocus;
		void onFocusUpdated() {
			if (!m_hasFocus && m_beingMoved) {
				release();
			}
		}
		using element::setDstrect;
		using element::getDstrect;
		using element::setAngle;
		using element::getAngle;
		/**
		 *	\brief Sets the item's texture
		 *
		 *	\param *texture The texture for the item to use
		 */
		void setTexture(SDL_Texture *texture) {
			m_texture = texture;
		}
		/**
		 *	\brief Gets the item's texture
		 *
		 *	\returns The texture of the item
		 */
		SDL_Texture* getTexture() {
			return m_texture;
		}
		/**
		 *	\brief Release/drop the item
		 */
		void release() {
			m_beingMoved = false;
			//Check if we were released on an itemHolder
			if (m_parentSet != nullptr) {
				std::array<element*, 2> focused = m_parentSet->getFocusedElements();
				itemHolder<T>* focusedSub = dynamic_cast<itemHolder<T>*>(focused[1]);
				if (focused[0] == this && focusedSub != nullptr) {
					focusedSub->setChild(this);
				}
			}
		}
		/**
		 *	\brief Pickup the item and attach it to the user's mouse
		 */
		void pickup() {
			m_beingMoved = true;
			//Check if we are being taken from an itemHolder
			if (m_holder != nullptr) {
				m_holder->setChild((item<T>*)nullptr);
				m_holder = nullptr;
				m_callEventFunction(Event::ValueChanged);
			}
		}

		using element::pointInElement;
	};
};