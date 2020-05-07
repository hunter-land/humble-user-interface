#pragma once

#include "./element.hpp"

#include <SDL.h>
#include <SDL_ttf.h>
#include <array>
#include <vector>
#include <string>

namespace hui {
	//Get length of UTF-8 string
	//Use SDL_utf8strlen() instead
		//static int strlen_utf8(std::string s) {
		//	int strlen = s.length(), utflen = 0, c = 0;
		//	for (int i = 0; i < strlen; i++) {
		//		c = s.at(i);
		//		if ((c >= 0x0001) && (c <= 0x007F)) utflen++;
		//		else if (c > 0x07FF) utflen += 3;
		//		else utflen += 2;
		//	}
		//	return utflen;
		//}

	//Move from a given byte to a grapheme index (In bytes)
	/**
	 *	\brief Given a starting byte, find the starting byte of the grapheme X grapheme away
	 */
	static size_t getUTF8GraphemeIndex(const std::string &s, int graphemeDistance = 0, size_t fromByte = 0);
	
	/**
	 *	\brief A text entry field
	 *
	 *	A text entry field, with CJK support*, for users to enter text
	 *	as they desire. Click and drag selecting, shift and arrow-key
	 *	selecting, prompt string, copying, pasting, and cutting are supported.
	 *	
	 *
	 *	As of now (2020-4-28) this element is just smahsed together, but works.
	 *	I am afraid to touch it but it really, REALLY needs refactoring.
	 *
	 *	\note * Candidate list windows do not show up. This is a known bug.
	 *	\note All strings used in this class are formatted with UTF-8.
	 */
	class textField : public element {
	protected:
		bool m_updateTextureNextRender = true; //!<Do we need to update the texture next render call?
		SDL_Texture *m_texture, *m_textureCommitedLeft, *m_textureComposition, *m_textureCommitedRight; //Texture which is rendered and its parts
		SDL_Surface *m_surfaceCommitedLeft, *m_surfaceComposition, *m_surfaceCommitedRight; //Surfaces for all 3 parts of text, one linked to m_texture
		
		std::array<Uint8, 7>	m_r = { 255, 255, 255, 255, 000, 192, 000 }, //!<Red values of components. \sa textField::Component
								m_g = { 255, 255, 255, 255, 120, 192, 000 }, //!<Green values of components. \sa textField::Component
								m_b = { 255, 255, 255, 255, 215, 192, 000 }, //!<Blue values of components. \sa textField::Component
								m_a = { 255, 255, 255, 255, 255, 192, 000 }; //!<Alpha values of components. \sa textField::Component
		TTF_Font *m_font; //!<Font being used
		std::array<int, 2> m_underlineLengths = {10, 10}; //Length and spacing of underline
		SDL_Rect m_commitedRectLeft, m_compositionRect, m_commitedRectRight, m_underlineRect, m_cursorRect, m_selectionRect; //Rectangles for spacing and formatting (Internal)

		bool m_typing = false; //!<Are we listening for typing actions
		bool m_selecting = false; //!<Is user click and dragging to select an area of text
		size_t m_compositionIndex = 0; //!<Where should the commited text be split for composition
		size_t m_cursorIndex = 0, m_compositionCursorIndex = 0; //Where is the cursor among the text, currently.
		size_t m_selectionIndexBegin = 0, m_selectionIndexEnd = 0, m_selectingFromIndex = 0;
		std::string m_commitedString = ""; //!<The text in the field (Excluding composition text)
		std::string m_compositionString = ""; //!<Composition text
		std::string m_defaultString = ""; //!<The string which is rendered when there is no text and not focused

		void m_updateTexture(SDL_Renderer *renderer); //!<Update m_texture to reflect current state

		Uint32 m_cursorTimer = 0; //!<Timer used for tracking the cursor blinking
	public:
		/*!
		*	\brief Components of a textField
		*/
		enum Component { CommitedText, CompositionText, CompositionUnderline, Cursor, SelectionBox, PromptText, Background };

		/**
		 *	\brief Construct a text field with a font
		 *
		 *	\param *font The font of the text.
		 */
		textField(TTF_Font *font);
		/**
		 *	\brief Construct the text field with a font, position, size, angle, and flip
		 *
		 *	\param *font The font of the text.
		 *	\param dstrect The rectangle to draw the text field in.
		 *	\param angle The angle of the text field.
		 *	\param flip The flip of the text field.
		 */
		textField(TTF_Font *font, const SDL_FRect dstrect, const double angle = 0, const SDL_RendererFlip flip = SDL_FLIP_NONE);
		~textField();

		void render(SDL_Renderer *renderer);
		void userLogic(std::vector<SDL_Event> &events, SDL_Renderer *renderer);
		void resetUserLogic();
		void loopLogic(Uint32 ms);
		void resetLoopLogic();

		/**
		 *	\brief Set the font to use for rendering
		 *
		 *	\param *font The font to be used.
		 */
		void setFont(TTF_Font *font);
		/**
		 *	\brief Get the current font
		 *
		 *	\return The font currently being used.
		 */
		TTF_Font* getFont();
		/**
		 *	\brief Set the line width
		 *
		 *	Line width is used for determining the width of the cursor
		 *	as well as the height of the underline components.
		 *
		 *	\param w Width to use.
		 */
		void setLineWidth(int w);
		/**
		 *	\brief Set the underlining length
		 *
		 *	Allows for setting a solid or dashed underline.
		 *
		 *	\param line Length of the lines
		 *	\param spacing Space between lines
		 */
		void setUnderlineLengths(int line, int spacing);
		/**
		 *	\brief Set the color of a specific component
		 *
		 *	\param clr Color to set the component to.
		 *	\param c Component to set the color for.
		 */
		void setColor(SDL_Color clr, Component c = CommitedText);
		/**
		 *	\brief Set the color of a specific component
		 *
		 *	\param r Value of red.
		 *	\param g Value of green.
		 *	\param b Value of blue.
		 *	\param a Value of alpha channel.
		 *	\param c Component to set the color for.
		 */
		void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a, Component c = CommitedText);
		/**
		 *	\brief Get the color of a specific component
		 *
		 *	\param c Component to set the color for.
		 *
		 *	\return The color of the given component.
		 */
		SDL_Color getColor(Component c = CommitedText);
		/**
		 *	\brief Get the color of a specific component
		 *
		 *	\param c Component to set the color for.
		 *
		 *	\return The color of the given component in array form. (RGBA)
		 */
		std::array<Uint8, 4> getColorArray(Component c = CommitedText);
		/**
		 *	\brief Set the string to be shown as a prompt
		 *
		 *	\param s UTF-8 string to use as a prompt.
		 */
		void setPromptString(std::string s);
		/**
		 *	\brief Get the string currently set to be shown as a prompt
		 *
		 *	\return The text which will be shown as a prompt.
		 */
		std::string getPromptString();
		/**
		 *	\brief Force the user to begin typing in this text field
		 *
		 *	This does not make the user stop typing in any other text fields and
		 *	should not typically be called externally unless there is good reason
		 *	to, such as a default selection when a new page is loaded.
		 */
		void startTyping();
		/**
		 *	\brief Is the user currently typing into this text field
		 *
		 *	\return True if the user is editing the text.
		 */
		bool isTyping();
		/**
		 *	\brief Stop typing in this text field
		 *
		 *	This does not make the user stop typing in any other text field and
		 *	should not need to be called externally unless you wish to force the
		 *	user to stop typing.
		 *
		 *	\note This function <b>will</b> hide the pull-up keyboard if being used.
		 */
		void stopTyping();
		/**
		 *	\brief Calculate the string character index based on an x position along the text field
		 *
		 *	\param x Distance along the text field to solve for.
		 *
		 *	\return Index of character at position.
		 *
		 *	\note x is relative to left of the text field.
		 *	\warning The returned index can be equal to the length of the string,
		 *	this indicates the point is past the end of the string.
		 */
		size_t indexFromPosition(Sint32 x);
		/**
		 *	\brief Set the commited string
		 *
		 *	The commited string is user-modifyable so this should only be
		 *	used for setting a default string when you <b>know</b> the user
		 *	will not be editing the text.
		 *
		 *	\param s UTF-8 string to set the commited text to.
		 *
		 *	\sa textField::isTyping()
		 */
		void setCommitedString(std::string s);
		/**
		 *	\brief Get the currently commited string
		 *
		 *	\return The string currently commited to the text field by user.
		 */
		std::string getCommitedString();
	};
};