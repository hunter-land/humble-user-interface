#pragma once

#include "./element.hpp"

#include <SDL.h>
#include <array>
#include <vector>
#include <mutex>

namespace hui {

	/**
	 *	\brief A collection visible elements
	 *
	 *	This is the container through which elements are managed and interacted with.
	 *	
	 *	Using the functions render(), userLogic(), resetUserLogic(), loopLogic(), and resetLoopLogic()
	 *	will cause the set to loop through all contained GUI elements, causing the same function to be
	 *	called for each element.
	 *
	 *	\note Render order is from firstly added element to lastly added element.
	 *	Logic order is the opposite of render order.
	 *	\note Sets can be nested, providing sub-windows such as a scrolling menu
	 *	<!--\note You should only be using one set at a time per window. (Sets CAN be nested as of version 0.3.0)-->
	 *	\note All elements must be inside of a set to work correctly.
	 *
	 *	\sa hui::scrollBar
	 */
	class set : public element {
	private:
		std::recursive_mutex m_elementsMutex; //Mutex for m_elements & m_focusedElements
			/**
			 *	The two top-most elements under the cursor
			 *	[0]: The top element. This element knows it is the top element.
			 *	[1]: The 2nd-top element. This element does NOT know it's Z position.
			 */
			std::array<element*, 2> m_focusedElements;
			std::vector<element*> m_elements;

		bool m_updateFocusElements = true; //Read, acted on, and set to false inside the `userLogic` function

		std::recursive_mutex m_textureMutex; //Mutex for m_texture and m_renderCorner
			SDL_Texture *m_texture = nullptr; //Texture, used only when acting as a subset to another set
			Uint8 m_r = 255, m_g = 255, m_b = 255, m_a = 0;
			SDL_FPoint m_renderCorner; //Corner, in local coordinates, to use as origin when rendering
	public:
		/**
		 *	\brief Construct the set
		 *
		 *	\param elements A vector of elements to put into the set.
		 */
		set(std::vector<element*> elements = {});
		/**
		 *	\brief Deconstruct set
		 */
		~set();

		/**
		 *	\brief Render all elements in the set
		 *
		 *	\param *renderer Renderer to use for rendering.
		 */
		void render(SDL_Renderer *renderer);
		/**
		*	\brief Update all elements based on events
		*
		*	\param &events Vector of SDL_Events to be processed this frame.
		*	\param *renderer Renderer the set is displayed on.
		*/
		void userLogic(std::vector<SDL_Event> &events, SDL_Renderer *renderer);
		/**
		*	\brief Reset the updates caused by events / userLogic()
		*/
		void resetUserLogic();
		/**
		*	\brief Update all elements based on time
		*
		*	\param ms How much time will be simulated, in milliseconds.
		*/
		void loopLogic(Uint32 ms);
		/**
		*	\brief Reset the updates caused by time / loopLogic()
		*/
		void resetLoopLogic();

		/**
		 *	\brief Update the backpointer to the parent set
		 *
		 *	\param s The set which we are a part of.
		 */
		void setSet(set* s);
		void onFocusUpdated();
		void setDstrect(SDL_FRect dr);
		using element::getDstrect;
		using element::setAngle;
		using element::getAngle;
		using element::setFlip;
		using element::getFlip;
		//TODO: Fix-up doxygen documentation
		/**
		 *	\brief Set the origin corner to render
		 *
		 *	\param shift The distance to shift the contained elements by before rendering
		 */
		void setRenderCorner(SDL_FPoint shift);
		//TODO: Fix-up doxygen documentation
		/**
		 *	\brief Get the origin corner to render
		 *
		 *	\return The origin corner
		 */
		SDL_FPoint getRenderCorner();
		/**
		 *	\brief Add an element to the set
		 *
		 *	\param *e element to add to the set.
		 *
		 *	\returns true if the element is added
		 */
		bool addElement(element *e);
		/**
		 *	\brief Add an element to the set
		 *
		 *	\param es elements to add to the set.
		 *
		 *	\returns number of elements added
		 */
		size_t addElement(std::vector<element*> es);
		/**
		 *	\brief Removes an element from the set
		 *
		 *	\param *e element to remove from the set.
		 *
		 *	\returns true if the element is removed
		 */
		bool removeElement(element *e);
		/**
		 *	\brief Removes an element from the set based on index
		 *
		 *	\param position Index to remove an element from.
		 *
		 *	\returns element which was removed from the set or nullptr if position is invalid
		 */
		element* removeElement(size_t position);
		/**
		 *	\brief Get a copy of the focused elements
		 *
		 *	\returns array of the focused elements
		 */
		std::array<element*, 2> getFocusedElements();
		/**
		 *	\brief Signal the set to update the focusedElements the next time userLogic() is called
		 */
		void setUpdateFocusElements();

		/**
		 *	\brief Convert coordinates from window space to local space
		 *
		 *	This function will convert a point from the window's coordinate system
		 *	to a point in the set's coordinate system.
		 *
		 *	\param windowPoint Point relative to the window to be converted.
		 *
		 *	\return Point in this set's coordinate system
		 *	\sa localToWindow
		 */
		SDL_FPoint windowToLocal(SDL_FPoint windowPoint);
		/**
		 *	\brief Convert coordinates from window space to local space
		 *
		 *	This function will convert a point from the window's coordinate system
		 *	to a point in the set's coordinate system.
		 *
		 *	\param windowPoint Point relative to the window to be converted.
		 *
		 *	\return Point in this set's coordinate system
		 *	\note The more nested a set is, the less accurate the result will be.
		 *	The result can become unusable after just a handful of layers.
		 *	It is recommended to use the floating version of the function instead.
		 *	\sa localToWindow
		 */
		SDL_Point windowToLocal(SDL_Point windowPoint);
		/**
		 *	\brief Convert coordinates from local space to window space
		 *
		 *	This function will convert a point from the local coordinate system
		 *	to a point in the window's coordinate system
		 *
		 *	\param localPoint Point relative to this set to be converted.
		 *
		 *	\return Point in the window's coordinate system
		 *	\sa windowToLocal
		 */
		SDL_FPoint localToWindow(SDL_FPoint localPoint);
		/**
		 *	\brief Convert coordinates from local space to window space
		 *
		 *	This function will convert a point from the local coordinate system
		 *	to a point in the window's coordinate system
		 *
		 *	\param localPoint Point relative to this set to be converted.
		 *
		 *	\return Point in the window's coordinate system
		 *	\note The more nested a set is, the less accurate the result will be.
		 *	The result can become unusable after just a handful of layers.
		 *	It is recommended to use the floating version of the function instead.
		 *	\sa windowToLocal
		 */
		SDL_Point localToWindow(SDL_Point localPoint);
	};
};