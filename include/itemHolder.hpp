#pragma once

#include "./element.hpp"

extern "C" {
#include <SDL2/SDL.h>
}
#include <vector>
#include "./extra.hpp" //RotatePoint

namespace lui {
	template<typename T>
	class item;

	/**
	 *	\brief A slot which accepts an item as a child.
	 *
	 *	\note ItemHolder template type must match the item's template type for the item to be held
	 *	\see hui::itemHolder#setAcceptFunction
	 */
	template<typename T>
	class itemHolder : public element {
	public:
		/**
		*	\brief The function type used to check if an item is valid
		*/
		typedef bool(*itemAcceptFunction)(itemHolder<T>*, item<T>*);
	protected:
		SDL_Texture * m_texture = nullptr; //!<The texture of the itemHolder
		item<T> *m_childItem = nullptr; //!<Item currently held by the itemHolder

		itemAcceptFunction m_acceptFunction = nullptr; //!<Function to determine if an item is valid for the itemHolder
	public:
		SDL_Rect *srcrect = nullptr; //!<Rectangle of the texture to draw

		/**
		 *	\brief Construct itemHolder with a texture and a function
		 *
		 *	\param *texture The texture of the itemHolder.
		 *	\param *func The function which will return if an item is valid or not.
		 */
		itemHolder(SDL_Texture *texture, bool(*func)(itemHolder<T>*, item<T>*) = nullptr) {
			setTexture(texture);
			setAcceptFunction(func);
		}
		/**
		 *	\brief Construct itemHolder with a texture, position, size, angle, and function
		 *
		 *	\param *texture The texture of the itemHolder.
		 *	\param dstrect The rectangle to draw the itemHolder in.
		 *	\param angle The angle the itemHolder will be at.
		 *	\param flip The flip the itemHolder will have.
		 *	\param *func The function which will return if an item is valid or not.
		 */
		itemHolder(SDL_Texture *texture, const SDL_FRect dstrect, const double angle = 0, const SDL_RendererFlip flip = SDL_FLIP_NONE, bool(*func)(itemHolder<T>*, item<T>*) = nullptr) {
			setDstrect(dstrect);
			setAngle(angle);
			setFlip(flip);
			setTexture(texture);
			setAcceptFunction(func);
		}
		/**
		 *	\brief Deconsturct itemHolder
		 */
		~itemHolder() {}
		
		void render(SDL_Renderer *renderer) {
			SDL_RenderCopyExF(renderer, m_texture, srcrect, &m_dstrect, m_angle, &zeroFPoint, m_flip);
		}
		using element::userLogic;
		using element::resetUserLogic;
		using element::loopLogic;
		using element::resetLoopLogic;
		using element::bind;
		using element::unbind;
		/**
		 *	\brief Sets the function to check if an item is valid for this holder
		 *
		 *	If the function returns true and the holder has no current child, the item will be accepted
		 *
		 *	\param func Function which is given
		 *
		 *	\note This function will also be called to check if a child can be removed.
		 *	\warning While the itemHolder<T>* paramter will never be a nullptr, the item<T>* parameter will be nullptr when checking if a child can be removed.
		 */
		void setAcceptFunction(bool(*func)(itemHolder<T>*, item<T>*)) {
			m_acceptFunction = func;
		}

		using element::setSet;
		using element::setFocus;
		void setDstrect(SDL_FRect dr) {
			element::setDstrect(dr);
			centerChild();
		}
		using element::getDstrect;
		void setAngle(double a) {
			element::setAngle(a);
			centerChild();
		}
		using element::getAngle;
		/**
		 *	\brief Sets the itemHolders's texture
		 *
		 *	\param *texture The texture for the itemHolder to use
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
		 *	\brief Attempts to set the child item to the given child
		 *
		 *	\param *child Item to attempt to place in the itemHolder.
		 *
		 *	\returns true if child was set
		 */
		template<typename U>
		bool setChild(item<U> *child) {
			if (std::is_same<T, U>::value) { //If they are not of same type, they can not (will not) be compared.
				if (m_acceptFunction == nullptr || m_acceptFunction != nullptr && m_acceptFunction(this, child)) { //If there is no accept function, all are welcome.
					if ((m_childItem == nullptr) != (child == nullptr)) {
						m_childItem = child;
						if (child != nullptr) { //No current child, and a valid child
							centerChild();
							m_childItem->m_holder = this;
						}
						m_callEventFunction(Event::ValueChanged);
						return true;
					}
				}
			}
			return false;
		}
		/**
		 *	\brief Gets the current child item
		 *
		 *	\returns The current child item
		 */
		item<T>* getChild() {
			return m_childItem;
		}
		/**
		 *	\brief Gets the value of the child item
		 *
		 *	\returns The child item's value if present, otherwise the default value for type T
		 *	\note Type T's default value it calculated using T().
		 */
		T getChildValue() {
			return m_childItem != nullptr ? m_childItem->value : T();
		}
		/**
		 *	\brief Update's the child item's position and angle to be centered in the itemHolder
		 */
		void centerChild() {
			if (m_childItem != nullptr) {
				SDL_FRect childDstrect = m_childItem->getDstrect();
				SDL_FPoint nominalPosition; //Centered position of child provided m_angle == 0

				nominalPosition.x = m_dstrect.x + (m_dstrect.w - childDstrect.w) / 2.f;
				nominalPosition.y = m_dstrect.y + (m_dstrect.h - childDstrect.h) / 2.f;

				SDL_FPoint upperLeft = RotatePoint(nominalPosition, m_angle, { m_dstrect.x, m_dstrect.y });
				
				childDstrect.x = upperLeft.x;
				childDstrect.y = upperLeft.y;
				m_childItem->setDstrect(childDstrect);
				m_childItem->setAngle(m_angle);
			}
		}

		using element::pointInElement;
	};
};
