#pragma once

#include <SDL.h>
#include <array>
#include <vector>
#include <functional>

/**
 *	\brief Humble User Interface namespace
 *
 *	All aspects of Humble User Interface and stored inside of the hui namespace for organization.
 */
namespace hui {
	
	/*!
	 *	\brief A list of events which an eventFunction can be bound to
	 *
	 *	These events are only valid when the user's mouse cursor is
	 *	directly above the element in question.
	 */
	enum Event {
		//Element:
		FocusGained,	//!<Just became current element
		FocusLost,		//!<Just stopped being current element
		ValueChanged,	//!<Value of the element has changed

		//Mouse buttons; These will likely be depricated and removed in the future:
		LMBDown,		//!<LMB was pressed down on element
		LMBUp,			//!<LMB was released on element
		MMBDown,		//!<MMB was pressed down on element
		MMBUp,			//!<MMB was released on element
		//MMBScrolled,	//!<MMB was scrolled on element
		RMBDown,		//!<RMB was pressed down on element
		RMBUp,			//!<RMB was released on element
		X1BDown,		//!<Mouse button 1 was pressed on element
		X1BUp,			//!<Mouse button 1 was released on element
		X2BDown,		//!<Mouse button 2 was pressed on element
		X2BUp,			//!<Mouse button 2 was released on element

		//Generics: 2nd argument will never be nullptr and function can be called on *any* element
		MouseDown,		//!<A mouse button was pressed on element
		MouseUp,		//!<A mouse button was released on element
		MouseMotion,		//!<The mouse has moved on element
		MouseWheel,	//!<The mouse wheel has scrolled on element
		KeyDown,		//!<A key was pressed on element
		KeyUp,			//!<A key was released on element

		//ValueChange,	//!<Value of the element has changed
		Size			//!<The number of valid Event options listed above. \note Do not use as option.
	};

	class element;
	class set;

	/**
	 *	\brief The function type of all event functions
	 */
	typedef std::function<void(element*, SDL_Event*)> eventFunction;
	//typedef void(*eventFunction)(element*); //hui event functions 

	/**
	 *	\brief Base-class inherited by all other GUI classes
	 *
	 *	\warning The element class is meant only for the inheritance of other classes and should not typically be instansiated on it's own.
	 */
	class element {
	protected:
		/**
		 *	\brief Destination Rectangle the element will be drawn in
		 */
		SDL_FRect m_dstrect; //Destination rectangle
		/**
		 *	\brief Angle in degrees the element will be rotated when rendered
		 *
		 *	\note The element is rotated around the upper-left corner of m_dstrect
		 */
		double m_angle = 0; //Angle
		/**
		 *	\brief The flip the element will have when rendered
		 */
		SDL_RendererFlip m_flip = SDL_FLIP_NONE; //Flip

		/**
		 *	\brief The hui this element is a part of
		 *
		 *	@see hui::set#setChild
		 */
		set* m_parentSet = nullptr; //The set we are a part of

		/**
		 *	\brief Array of all event function pointers
		 */
		std::array<eventFunction, Event::Size> m_eventFunctions; //All bound functions, index corosponding to the hui::Event value
		/**
		 *	\brief Function to safely call a bound function based on an event
		 *
		 *	\param e What eventFunctions to call.
		 *	\param sdle SDL_Event paired with the event.
		 *
		 *	\return true if an eventFunction was called successfully
		 */
		bool m_callEventFunction(Event e, SDL_Event *sdle = nullptr) {
			if (e < Event::Size && m_eventFunctions[(size_t)e] != nullptr) {
				m_eventFunctions[(size_t)e](this, sdle);
				return true; //Ran function properly
			} else {
				return false; //Could not reach a function
			}
		}
		/**
		 *	\brief Does this element have focus
		 */
		bool m_hasFocus = false;
	public:
		/**
		 *	\brief Construct the element a size of 0x0
		 */
		element();
		/**
		 *	\brief Construct the element
		 *
		 *	\param dstrect Rectangle to draw the element in on screen.
		 *	\param angle Angle in degrees the element will be rotated.
		 *	\param flip Flip orientation the element will have.
		 */
		element(const SDL_FRect dstrect, const double angle = 0, const SDL_RendererFlip flip = SDL_FLIP_NONE);
		/**
		 *	\brief Destruct element
		 */
		~element();

		/**
		 *	\brief Render the element
		 *
		 *	\param *renderer Renderer to use for rendering.
		 */
		virtual void render(SDL_Renderer *renderer);
		/**
		 *	\brief Update the element based on events
		 *
		 *	\param &events Vector of SDL_Events to be processed this frame.
		 *	\param *renderer Renderer the element is displayed on.
		 */
		virtual void userLogic(std::vector<SDL_Event> &events, SDL_Renderer *renderer);
		/**
		 *	\brief Reset the updates caused by the events / userLogic()
		 */
		virtual void resetUserLogic();
		/**
		 *	\brief Update the element based on time
		 *
		 *	\param ms How much time will be simulated, in milliseconds.
		 */
		virtual void loopLogic(Uint32 ms);
		/**
		 *	\brief Reset the updates caused by time / loopLogic()
		 */
		virtual void resetLoopLogic();
		/**
		 *	\brief Bind a function to a specific event
		 *
		 *	\param e The event which will call the function.
		 *	\param func Function pointer to be fired on event e.
		 *
		 *	\return true if function was bound.
		 */
		virtual bool bind(Event e, eventFunction func);
		/**
		 *	\brief Unbind all functions for a specific event
		 *
		 *	\param e The even to unbind functions for.
		 *
		 *	\return true if function was unbound.
		 */
		virtual bool unbind(Event e);

		/**
		 *	\brief Update the backpointer to the parent set
		 *
		 *	\param s The set which we are a part of.
		 */
		void setSet(set* s);
		/**
		 *	\brief Tell this element if it has focus or not
		 *
		 *	\param hasFocus Should this element have focus.
		 */
		void setFocus(bool hasFocus);
		/**
		 *	\brief A function which is fired whenever m_hasFocus changed
		 */
		virtual void onFocusUpdated();
		/**
		*	\brief Set the dstrect of the element
		*
		*	\param dr Rectangle to renderer the element in
		*/
		virtual void setDstrect(SDL_FRect dr);
		/**
		*	\brief Gets the current dstrect of the element
		*
		*	\return The dstrect of the element
		*/
		virtual SDL_FRect getDstrect();
		/**
		 *	\brief Set the angle of the element
		 *
		 *	\param a Angle in degrees to set the element
		 */
		virtual void setAngle(double a);
		/**
		 *	\brief Gets the current angle of the element
		 *
		 *	\return The angle of the element in degrees
		 */
		virtual double getAngle();
		/**
		 *	\brief Set the flip of the element
		 *
		 *	\param flip The flip to apply.
		 */
		virtual void setFlip(SDL_RendererFlip flip);
		/**
		 *	\brief Get the current flip of the element
		 *
		 *	\return The flip of the element
		 */
		virtual SDL_RendererFlip getFlip();

		/**
		*	\brief Is a point inside the interface
		*
		*	\param point Point to check for.
		*
		*	\return true if point resides inside interface
		*/
		virtual bool pointInElement(const SDL_Point point);
	};

	const extern SDL_Point zeroPoint; ///<A SDL_Point at (0, 0)
	const extern SDL_FPoint zeroFPoint; ///<A SDL_FPoint at (0.f, 0.f)
};