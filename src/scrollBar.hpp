#pragma once

#include "./element.hpp"

#include <SDL.h>

namespace hui {
	/**
	 *	\brief A scroll bar with 2D functionality
	 *
	 *	This scroll bar offers 2D functionality, but can be used as a vertical or
	 *	horizontal scroll bar by making the grip just as wide as the track in one
	 *	axis.
	 *
	 *	\note The grip will never be larger than the track
	 *
	 *	\sa hui::set
	 */
	class scrollBar : public element {
	protected:
		SDL_Texture *m_textureTrack = nullptr; //!<The texture of the track
		SDL_Texture *m_textureGrip = nullptr; //!<The texture of the grip

		bool m_gripMoving = 0; //!<Is the grip being dragged by user
		SDL_FRect m_dstrectGrip; //!<Rectangle to draw the grip in
		SDL_FPoint m_gripPos = {0.f, 0.f}; //!<The position of the grip and all attached scrollWindows

		/**
		 *	\brief Array of all event function pointers
		 */
		std::array<eventFunction, Event::Size> m_gripEventFunctions;
		/**
		 *	\brief Function to safely call a function bound to the track based on an event
		 *
		 *	\param e What eventFunctions to call.
		 *	\param sdle SDL_Event paired with the event.
		 *
		 *	\return true if an eventFunction was called successfully
		 */
		bool m_callGripEventFunction(Event e, SDL_Event *sdle = nullptr) {
			if (e < Event::Size && m_gripEventFunctions[(size_t)e] != nullptr) {
				m_gripEventFunctions[(size_t)e](this, sdle);
				return true; //Ran function properly
			}
			else {
				return false; //Could not reach a function
			}
		}
		/**
		 *	\brief Does the grip have focus
		 */
		bool m_gripHasFocus = false;
		bool m_updateGripFocus = true; //!<Update m_gripHasFocus next time userLogic() is called
	public:
		/*!
		 *	\brief Components of a scrollBar
		 */
		enum Component { Track, Grip };
		SDL_Rect *srcrectTrack = nullptr; //!<Rectangle of the track texture to draw
		SDL_Rect *srcrectGrip = nullptr; //!<Rectangle of the grip texture to draw

		/**
		 *	\brief Construct scrollBar with two textures
		 *
		 *	\param *trackTexture The texture for the track.
		 *	\param *gripTexture The texture for the grip.
		 */
		scrollBar(SDL_Texture *trackTexture, SDL_Texture *gripTexture);
		/**
		 *	\brief Construct scrollBar with two textures, a position and size for the track, a position and size for the grip (relative), and angle
		 *
		 *	\param *trackTexture The texture for the track.
		 *	\param *gripTexture The texture for the grip.
		 *	\param dstrectTrack The rectangle to draw the scrollBar in
		 *	\param dstrectGrip The rectangle to draw the grip in (Relative to dstrect)
		 *	\param angle The angle of the scrollBar
		 */
		scrollBar(SDL_Texture *trackTexture, SDL_Texture *gripTexture, const SDL_FRect dstrectTrack, const SDL_FRect dstrectGrip, const double angle = 0);
		/**
		 *	\brief Deconstruct scroll bar
		 */
		~scrollBar();

		void render(SDL_Renderer *renderer);
		void userLogic(std::vector<SDL_Event> &events, SDL_Renderer *renderer);
		void resetUserLogic();
		using element::loopLogic;
		using element::resetLoopLogic;
		/**
		 *	\brief Bind a function to a specific event
		 *
		 *	\param e The event which will call the function.
		 *	\param func Function pointer to be fired on event e.
		 *	\param c The part of the scrollBar to bind to.
		 *
		 *	\return true if function was bound.
		 */
		bool bind(Event e, eventFunction func, Component c = Component::Grip);
		/**
		 *	\brief Unbind all functions for a specific event and component
		 *
		 *	\param e The even to unbind functions for.
		 *	\param c The component to unbind from.
		 *
		 *	\return true if function was unbound.
		 */
		bool unbind(Event e, Component c = Component::Grip);

		using element::setFocus;
		void onFocusUpdated();
		void setDstrect(SDL_FRect dr);
		using element::getDstrect;
		void setAngle(double a);
		using element::getAngle;
		/**
		 *	\brief Sets the tracks's texture
		 *
		 *	\param *texture The texture for the track to use
		 */
		void setTrackTexture(SDL_Texture *texture);
		/**
		 *	\brief Gets the track's texture
		 *
		 *	\returns The texture of the track
		 */
		SDL_Texture* getTrackTexture();
		/**
		 *	\brief Sets the grip's texture
		 *
		 *	\param *texture The texture for the grip to use
		 */
		void setGripTexture(SDL_Texture *texture);
		/**
		 *	\brief Gets the grip's texture
		 *
		 *	\returns The texture of the grip
		 */
		SDL_Texture* getGripTexture();
		/**
		 *	\brief Set the grip's position and size
		 *
		 *	\param grprect
		 *	\note Position is relative to the position of the track
		 */
		void setGripDstrect(SDL_FRect grprect);
		/**
		 *	\brief Get the grip's position and size
		 *
		 *	\returns Position and size of the grip
		 *	\note Position is \b NOT relative to the position of the track
		 */
		SDL_FRect getGripDstrect();
		/**
		 *	\brief Get the position of the grip
		 *
		 *	Sets the current position of the grip and all attached scrollWindows.
		 *
		 *	\param pos The position to set the grip to.
		 *
		 *	\note Position coordinates must be a % between min and max position
		 */
		void setGripPosition(SDL_FPoint pos);
		/**
		 *	\brief Get the position of the grip
		 *
		 *	\returns The position of the grip
		 *	\note Position is given as % between min and max position
		 */
		SDL_FPoint getGripPosition();

		using element::pointInElement;
		/**
		 *	\brief Identical to pointInElement() but for the grip component
		 *
		 *	\param point Point to check against.
		 */
		bool pointInGrip(SDL_Point point);
	};
};