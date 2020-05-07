#include "./textField.hpp"
#include <iostream>
#include "./extra.hpp"

using namespace hui;

textField::textField(TTF_Font *font) {
	setFont(font);
	setLineWidth(5);
}
textField::textField(TTF_Font *font, const SDL_FRect dstrect, const double angle, const SDL_RendererFlip flip) {
	setFont(font);
	setLineWidth(5);
	setDstrect(dstrect);
	setAngle(angle);
	setFlip(flip);
}
textField::~textField() {
	SDL_FreeSurface(m_surfaceCommitedLeft);
	SDL_FreeSurface(m_surfaceComposition);
	SDL_FreeSurface(m_surfaceCommitedRight);
	SDL_DestroyTexture(m_texture);
	SDL_DestroyTexture(m_textureCommitedLeft);
	SDL_DestroyTexture(m_textureComposition);
	SDL_DestroyTexture(m_textureCommitedRight);
}

void textField::render(SDL_Renderer *renderer) {
	if (m_updateTextureNextRender) {
		m_updateTexture(renderer);
	}
	SDL_RenderCopyExF(renderer, m_texture, NULL, &m_dstrect, m_angle, &zeroFPoint, m_flip);
}
void textField::userLogic(std::vector<SDL_Event> &events, SDL_Renderer *renderer) {
	element::userLogic(events, renderer);
	for (SDL_Event &e : events) {
		if (m_hasFocus) {
			switch (e.type) {
			case SDL_MOUSEBUTTONDOWN:
				if (e.button.button == SDL_BUTTON_LEFT) { //Dragging now
					m_selecting = true;
					//Translate point to be relative to the element:
					SDL_FPoint originalPoint{ e.button.x - m_dstrect.x, e.button.y - m_dstrect.y };
					SDL_FPoint localPoint = RotatePoint(originalPoint, -m_angle);
					m_selectingFromIndex = indexFromPosition(localPoint.x);
					m_selectionIndexBegin = 0;
					m_selectionIndexEnd = 0;
					//std::cout << "starting selection from " << m_selectingFromIndex << std::endl;
					m_cursorIndex = m_selectingFromIndex;
					m_updateTextureNextRender = true;
				}
				break;
			case SDL_MOUSEMOTION:
				if (m_selecting) {
					//Translate point to be relative to the element:
					SDL_FPoint originalPoint{ e.button.x - m_dstrect.x, e.button.y - m_dstrect.y };
					SDL_FPoint localPoint = RotatePoint(originalPoint, -m_angle);
					m_cursorIndex = indexFromPosition(localPoint.x);
					if (m_selectingFromIndex < m_cursorIndex) {
						m_selectionIndexBegin = m_selectingFromIndex;
						m_selectionIndexEnd = m_cursorIndex;
					} else {
						m_selectionIndexBegin = m_cursorIndex;
						m_selectionIndexEnd = m_selectingFromIndex;
					}
					//std::cout << "Selection [" << m_selectionIndexBegin << ":" << m_selectionIndexEnd << ")" << std::endl;
					m_updateTextureNextRender = true;
				}
				break;
			case SDL_MOUSEBUTTONUP: //Clicking to set cursor position (and start typing())
									//Clicking to select an area (No composition allowed when doing this)
				if (e.button.button == SDL_BUTTON_LEFT) {
					m_selecting = false;
					startTyping();
					m_updateTextureNextRender = true;
					//std::cout << "UP" << std::endl;
				}
				break;
			}
		}
		else {
			if (m_typing && e.type == SDL_MOUSEBUTTONDOWN) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					m_updateTextureNextRender = true;
					stopTyping();
				}
			}
		}
		if (m_typing) {
			switch (e.type) {
			case SDL_TEXTINPUT: //User has added text to the commited string, insert it at the cursor position
				if (e.text.text[0] == 1) {
					break;
				}
				//std::cout << "User commited some text!" << std::endl << e.text.text << std::endl;
				if (m_selectionIndexBegin != m_selectionIndexEnd) {
					m_commitedString.erase(m_selectionIndexBegin, m_selectionIndexEnd - m_selectionIndexBegin);
					m_cursorIndex = m_selectionIndexBegin;
					m_selectionIndexBegin = 0;
					m_selectionIndexEnd = 0;
				}
				m_commitedString.insert(m_cursorIndex, e.text.text);
				m_cursorIndex += std::strlen(e.text.text);
				m_compositionIndex = 0;
				m_updateTextureNextRender = true;
				m_cursorTimer = 0;

				m_callEventFunction(ValueChanged);
				break;
			case SDL_TEXTEDITING:
				if (m_compositionString == "") { //Just started editing
					if (m_selectionIndexBegin != m_selectionIndexEnd) {
						m_commitedString.erase(m_selectionIndexBegin, m_selectionIndexEnd - m_selectionIndexBegin);
						m_cursorIndex = m_selectionIndexBegin;
						m_selectionIndexBegin = 0;
						m_selectionIndexEnd = 0;
					}
					m_compositionIndex = m_cursorIndex;
				}
				m_compositionString = std::string(e.edit.text);
				m_compositionCursorIndex = getUTF8GraphemeIndex(m_compositionString, e.edit.start, 0);
				m_updateTextureNextRender = true;
				m_cursorTimer = 0;
				break;
			case SDL_KEYDOWN: //Backspace and delete
				if (m_compositionString == "") { //Not editing composition
					switch (e.key.keysym.sym) {
					case SDLK_BACKSPACE: //Delete UTF8 char to the left of cursor
						if (m_cursorIndex > 0) {
							if (m_selectionIndexBegin == m_selectionIndexEnd) {
								size_t deleteIndex = getUTF8GraphemeIndex(m_commitedString, -1, m_cursorIndex);
								m_commitedString.erase(deleteIndex, m_cursorIndex - deleteIndex); //TODO: Length should not be one, but instead be the byte difference from current to next grapheme
								m_cursorIndex -= m_cursorIndex - deleteIndex;
							} else {
								//Delete selected text
								m_commitedString.erase(m_selectionIndexBegin, m_selectionIndexEnd - m_selectionIndexBegin);
								m_selectionIndexBegin = 0;
								m_selectionIndexEnd = 0;
							}
							m_updateTextureNextRender = true;
							m_cursorTimer = 0;
						}
						break;
					case SDLK_DELETE: //Delete UTF8 char to the right of cusror
						if (m_cursorIndex < m_commitedString.size()) {
							if (m_selectionIndexBegin == m_selectionIndexEnd) {
								size_t deleteIndex = getUTF8GraphemeIndex(m_commitedString, 1, m_cursorIndex);
								m_commitedString.erase(m_cursorIndex, deleteIndex - m_cursorIndex); //TODO: Length should not be one, but instead be the byte difference from current to next grapheme
							} else {
								//Delete selected text
								m_commitedString.erase(m_selectionIndexBegin, m_selectionIndexEnd - m_selectionIndexBegin);
								m_selectionIndexBegin = 0;
								m_selectionIndexEnd = 0;
							}
							m_updateTextureNextRender = true;
							m_cursorTimer = 0;
						}
						break;
					case SDLK_LEFT: //Move cursor one UTF8 char left
					{
						//std::cout << "Cursor moved from byte " << m_cursorIndex << " to ";
						size_t newCursorIndex = getUTF8GraphemeIndex(m_commitedString, -1, m_cursorIndex);
						if (e.key.keysym.mod & KMOD_SHIFT != 0) { //If we are selecting text, move both pointers accordingly
							//One index should be at cursor position, the other one should be where it was
							if (m_selectionIndexBegin == m_selectionIndexEnd) { //If they were ontop of eachother, move the lower one to cursorIndex
								m_selectionIndexBegin = newCursorIndex;
								m_selectionIndexEnd = m_cursorIndex;
							} else {
								if (m_cursorIndex == m_selectionIndexBegin) {
									m_selectionIndexBegin = newCursorIndex;
								} else {
									m_selectionIndexEnd = newCursorIndex;
								}
							}
						} else {
							m_selectionIndexBegin = 0;
							m_selectionIndexEnd = 0;
						}
						m_cursorIndex = newCursorIndex;
						m_updateTextureNextRender = true;
						m_cursorTimer = 0;
						//std::cout << m_cursorIndex << std::endl;
					}
						break;
					case SDLK_RIGHT: //Move cursor one UTF8 char right
					{
						//std::cout << "Cursor moved from byte " << m_cursorIndex << " to ";
						size_t newCursorIndex = getUTF8GraphemeIndex(m_commitedString, 1, m_cursorIndex);
						if (e.key.keysym.mod & KMOD_SHIFT != 0) { //If we are selecting text, move both pointers accordingly
																  //One index should be at cursor position, the other one should be where it was
							if (m_selectionIndexBegin == m_selectionIndexEnd) { //If they were ontop of eachother, move the upper one to cursorIndex
								m_selectionIndexBegin = m_cursorIndex;
								m_selectionIndexEnd = newCursorIndex;
							} else {
								if (m_cursorIndex == m_selectionIndexBegin) {
									m_selectionIndexBegin = newCursorIndex;
								} else {
									m_selectionIndexEnd = newCursorIndex;
								}
							}
						} else {
							m_selectionIndexBegin = 0;
							m_selectionIndexEnd = 0;
						}
						m_cursorIndex = newCursorIndex;
						m_updateTextureNextRender = true;
						m_cursorTimer = 0;
						//std::cout << m_cursorIndex << std::endl;
					}
						break;
					case SDLK_x: //Cut
						if ((e.key.keysym.mod & KMOD_CTRL) != 0 && m_selectionIndexBegin < m_selectionIndexEnd) { //Ctrl+X = Cut
							SDL_SetClipboardText(m_commitedString.substr(m_selectionIndexBegin, m_selectionIndexEnd - m_selectionIndexBegin).data()); //Copy
							m_commitedString.erase(m_selectionIndexBegin, m_selectionIndexEnd - m_selectionIndexBegin); //Erase
							m_cursorIndex = m_selectionIndexBegin;
							m_selectionIndexBegin = 0;
							m_selectionIndexEnd = 0;
							m_updateTextureNextRender = true;
						}
						break;
					case SDLK_c: //Copy
						if ((e.key.keysym.mod & KMOD_CTRL) != 0 && m_selectionIndexBegin < m_selectionIndexEnd) {
							SDL_SetClipboardText(m_commitedString.substr(m_selectionIndexBegin, m_selectionIndexEnd - m_selectionIndexBegin).data()); //Copy
						}
						break;
					case SDLK_v: //Paste
						if ((e.key.keysym.mod & KMOD_CTRL) != 0) {
							if (m_selectionIndexBegin < m_selectionIndexEnd) {
								m_commitedString.erase(m_selectionIndexBegin, m_selectionIndexEnd - m_selectionIndexBegin); //Erase
								m_cursorIndex = m_selectionIndexBegin;
								m_selectionIndexBegin = 0;
								m_selectionIndexEnd = 0;
							}
							char* clipText = SDL_GetClipboardText();
							m_commitedString.insert(m_cursorIndex, clipText);
							m_cursorIndex += std::strlen(clipText);
							m_updateTextureNextRender = true;
						}
						break;
					}
				}
				break;
			}
		}
	}
}
void textField::resetUserLogic() {
	//Clear all text
	m_commitedString = "";
	m_compositionString = "";
	m_cursorIndex = 0;
	m_selectionRect.w = 0;
	m_selectionRect.h = 0;
	m_updateTextureNextRender = true;
}
void textField::loopLogic(Uint32 ms) {
	Uint32 cursorTimerNew = (m_cursorTimer + ms) % 1500;
	if (m_cursorTimer < 750 && cursorTimerNew >= 750) { //Cursor became invisible, update texture
		m_updateTextureNextRender = true;
	} else if (m_cursorTimer >= 750 && cursorTimerNew < 750) { //Cursor became visible
		m_updateTextureNextRender = true;
	}
	m_cursorTimer = cursorTimerNew;
}
void textField::resetLoopLogic() {

}

void textField::setFont(TTF_Font *font) {
	m_font = font;
}
TTF_Font* textField::getFont() {
	return m_font;
}
void textField::setLineWidth(int w) {
	m_cursorRect.w = w;
	m_underlineRect.h = w;
}
void textField::setUnderlineLengths(int line, int blank) {
	m_underlineLengths[0] = line;
	m_underlineLengths[1] = blank;
}
void textField::setColor(SDL_Color clr, Component c) {
	m_r[c] = clr.r;
	m_g[c] = clr.g;
	m_b[c] = clr.b;
	m_a[c] = clr.a;
}
void textField::setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a, Component c) {
	m_r[c] = r;
	m_g[c] = g;
	m_b[c] = b;
	m_a[c] = a;
}
SDL_Color textField::getColor(Component c) {
	SDL_Color clr;
	clr.r = m_r[c];
	clr.g = m_g[c];
	clr.b = m_b[c];
	clr.a = m_a[c];
	return clr;
}
std::array<Uint8, 4> textField::getColorArray(Component c) {
	return { m_r[c], m_g[c], m_b[c], m_a[c] };
}
void textField::setPromptString(std::string s) {
	m_defaultString = s;
}
std::string textField::getPromptString() {
	return m_defaultString;
}
void textField::startTyping() {
	SDL_StartTextInput();
	m_typing = true;
	m_cursorTimer = 0;
}
bool textField::isTyping() {
	return m_typing;
}
void textField::stopTyping() {
	SDL_StopTextInput();
	m_typing = false;
	m_selectionIndexBegin = 0;
	m_selectionIndexEnd = 0;
	if (m_compositionString != "") {
		m_commitedString.insert(m_compositionIndex, m_compositionString);
		m_compositionString = "";
	}
}
size_t textField::indexFromPosition(Sint32 x) {
	//std::string totalString = m_commitedString.insert(m_compositionIndex, m_compositionString);
	int length = 0;
	for (size_t i = 0; i < m_commitedString.size(); i++) {
		TTF_SizeUTF8(m_font, m_commitedString.substr(0, i + 1).data(), &length, nullptr);
		if (length >= x) {
			return i;
		}
	}
	return m_commitedString.size();
}
void textField::setCommitedString(std::string s) {
	if (!m_typing) {
		m_commitedString = s;
	}
}
std::string textField::getCommitedString() {
	return m_commitedString;
}



void textField::m_updateTexture(SDL_Renderer *renderer) {
	//std::cout << "Updating texture" << std::endl;

	m_updateTextureNextRender = false;
	//Underline and composition size
	TTF_SizeUTF8(m_font, m_compositionString.data(), &m_compositionRect.w, &m_compositionRect.h);
	m_underlineRect.w = m_compositionRect.w;
	m_underlineRect.y = m_compositionRect.h;

	//Selection box position
	std::string preSelectionString = m_commitedString.substr(0, m_selectionIndexBegin);
	TTF_SizeUTF8(m_font, preSelectionString.data(), &m_selectionRect.x, nullptr);

	//Selection box size
	std::string selectionString = m_commitedString.substr(m_selectionIndexBegin, m_selectionIndexEnd - m_selectionIndexBegin);
	TTF_SizeUTF8(m_font, selectionString.data(), &m_selectionRect.w, &m_selectionRect.h);

	//commitedLeft size, composition and underline position
	std::string preCompositionString;
	if (!m_typing && m_commitedString == "") { //Show the default text / prompt
		preCompositionString = m_defaultString;
		TTF_SizeUTF8(m_font, m_defaultString.data(), &m_commitedRectLeft.w, &m_commitedRectLeft.h);
	} else if (m_compositionString != "") { //Composing some stuff, split it there
		preCompositionString = m_commitedString.substr(0, m_compositionIndex);
		TTF_SizeUTF8(m_font, preCompositionString.data(), &m_commitedRectLeft.w, &m_commitedRectLeft.h);
	} else {
		preCompositionString = m_commitedString;
		TTF_SizeUTF8(m_font, m_commitedString.data(), &m_commitedRectLeft.w, &m_commitedRectLeft.h);
	}
	m_compositionRect.x = m_commitedRectLeft.x + m_commitedRectLeft.w;
	m_underlineRect.x = m_commitedRectLeft.x + m_commitedRectLeft.w;

	//Cursor position
	if (m_compositionString == "") { //No composition
		//std::cout << "\tNo composition, rendering cursor position..." << std::endl;
		std::string preCursorString = m_commitedString.substr(0, m_cursorIndex);
		TTF_SizeUTF8(m_font, preCursorString.data(), &m_cursorRect.x, &m_cursorRect.h);
	} else { //Composition
		//std::cout << "\tComposition found, rendering cursor position within composition..." << std::endl;
		std::string preCursorString = m_compositionString.substr(0, m_compositionCursorIndex);
		TTF_SizeUTF8(m_font, preCursorString.data(), &m_cursorRect.x, &m_cursorRect.h);
		m_cursorRect.x += m_commitedRectLeft.w;
	}
	m_cursorRect.x += m_commitedRectLeft.x + m_cursorRect.w / 2;
	//If the cursor is off screen, adjust our rendering offset to account for it to be *just* on the edge
	//TODO: This

	//Done by this point: Selection Box, Cursor, CommitedLeft, Underline, Composition
	//Not done by this point: CommitedRight, Cursor-Correction

	m_commitedRectRight.x = m_compositionRect.x + m_compositionRect.w;
	std::string postCompositionString;
	if (m_compositionString != "") {
		postCompositionString = m_commitedString.substr(m_compositionIndex); //Everything past composition
	} else {
		postCompositionString = ""; //Nothing, we only use left
	}
	TTF_SizeUTF8(m_font, postCompositionString.data(), &m_commitedRectRight.w, &m_commitedRectRight.h);

	if (m_commitedRectLeft.w + m_compositionRect.w + m_commitedRectRight.w < m_dstrect.w) { //Keep 0 aligned, it fits in dstrect already
		m_commitedRectLeft.x = 0;
	} else {
		if (m_cursorRect.x > m_dstrect.w) { //If cursor is too far right, shift left
			m_commitedRectLeft.x -= m_cursorRect.x - m_dstrect.w;
			//m_updateTexture(renderer);
			//return;
		}
		else if (m_cursorRect.x < 0) { //If cursor is too far left, shift right
			m_commitedRectLeft.x -= m_cursorRect.x;
			//m_updateTexture(renderer);
			//return;
		}
		else if (m_commitedRectRight.x + m_commitedRectRight.w < m_dstrect.w) { //Right's end is too far left, shift right so max text is shown
			m_commitedRectLeft.x += m_dstrect.w - (m_commitedRectRight.x + m_commitedRectRight.w);
			//m_updateTexture(renderer);
			//return;
		}
	}

	//Now the rects are all correct (except for non-valid data points).
	//So now we recreate all the surfaces using the currently known text with m_font
	SDL_FreeSurface(m_surfaceCommitedLeft);
	SDL_FreeSurface(m_surfaceComposition);
	SDL_FreeSurface(m_surfaceCommitedRight);
	if (m_compositionString == "") { //All text goes in left, right is empty
		//std::cout << "\tNo composition, rendering all text in commitedLeft..." << std::endl;
		//std::cout << "\tFocus:" << m_hasFocus << "Typing:" << m_typing << "CommitedString:" << m_commitedString.data() << std::endl;
		if (!m_typing && m_commitedString == "") { //If we should show the default text / prompt
			//std::cout << "\t\tWriting default string \"" << m_defaultString.data() << "\"..." << std::endl;
			m_surfaceCommitedLeft = TTF_RenderUTF8_Solid(m_font, m_defaultString.data(), SDL_Color{ m_r[PromptText], m_g[PromptText], m_b[PromptText], m_a[PromptText] });
		} else {
			//std::cout << "\t\tWriting commited string \"" << m_commitedString.data() << "\"..." << std::endl;
			m_surfaceCommitedLeft = TTF_RenderUTF8_Solid(m_font, m_commitedString.data(), SDL_Color{ m_r[CommitedText], m_g[CommitedText], m_b[CommitedText], m_a[CommitedText] });
		}
		m_surfaceComposition = nullptr;
		m_surfaceCommitedRight = nullptr;
	} else {
		//std::cout << "\tComposition found, rendering text in 3 chunks..." << std::endl;
		m_surfaceCommitedLeft = TTF_RenderUTF8_Solid(m_font, preCompositionString.data(), SDL_Color{ m_r[CommitedText], m_g[CommitedText], m_b[CommitedText], m_a[CommitedText] });
		m_surfaceComposition = TTF_RenderUTF8_Solid(m_font, m_compositionString.data(), SDL_Color{ m_r[CompositionText], m_g[CompositionText], m_b[CompositionText], m_a[CompositionText]});
		m_surfaceCommitedRight = TTF_RenderUTF8_Solid(m_font, postCompositionString.data(), SDL_Color{ m_r[CommitedText], m_g[CommitedText], m_b[CommitedText], m_a[CommitedText] });
	}

	//Surfaces are finished now and are ready. Please watch warmly until the texture is ready.
	SDL_DestroyTexture(m_texture); //Won't be needing these anymore
	SDL_DestroyTexture(m_textureCommitedLeft);
	SDL_DestroyTexture(m_textureComposition);
	SDL_DestroyTexture(m_textureCommitedRight);

	m_textureCommitedLeft = SDL_CreateTextureFromSurface(renderer, m_surfaceCommitedLeft);
	m_textureComposition = SDL_CreateTextureFromSurface(renderer, m_surfaceComposition);
	m_textureCommitedRight = SDL_CreateTextureFromSurface(renderer, m_surfaceCommitedRight);

	m_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, std::round(m_dstrect.w), std::round(m_dstrect.h));
	SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);

		//Store previous things we will be changing for our texture rendering
		SDL_Texture *prevRenderTarget = SDL_GetRenderTarget(renderer);
		Uint8 prevR, prevG, prevB, prevA;
		SDL_GetRenderDrawColor(renderer, &prevR, &prevG, &prevB, &prevA);
		SDL_SetRenderTarget(renderer, m_texture);

	//Clear texture
	SDL_SetRenderDrawColor(renderer, m_r[Background], m_g[Background], m_b[Background], m_a[Background]);
	SDL_RenderClear(renderer);
	//Render the selection box first
	SDL_SetRenderDrawColor(renderer, m_r[SelectionBox], m_g[SelectionBox], m_b[SelectionBox], m_a[SelectionBox]);
	SDL_RenderFillRect(renderer, &m_selectionRect);
	//Composition underline second
	SDL_SetRenderDrawColor(renderer, m_r[CompositionUnderline], m_g[CompositionUnderline], m_b[CompositionUnderline], m_a[CompositionUnderline]);
	if (m_underlineLengths[1] <= 0) {
		SDL_RenderFillRect(renderer, &m_underlineRect);
	}
	else if (m_underlineLengths[0] > 0 && m_underlineLengths[1] > 0) {
		SDL_Rect dashRect = m_underlineRect;
		dashRect.w = m_underlineLengths[0];
		for (; dashRect.x < m_underlineRect.x + m_underlineRect.w; dashRect.x += m_underlineLengths[0] + m_underlineLengths[1]) {
			if (dashRect.x + dashRect.w > m_underlineRect.x + m_underlineRect.w) {
				dashRect.w = (dashRect.x + dashRect.w) - (m_underlineRect.x + m_underlineRect.w);
			}
			SDL_RenderFillRect(renderer, &dashRect);
		}
	}
	//Then the various texts
	SDL_RenderCopy(renderer, m_textureCommitedLeft, NULL, &m_commitedRectLeft);
	//std::cout << m_commitedRectLeft.x << " for " << m_commitedRectLeft.w << std::endl;
	SDL_RenderCopy(renderer, m_textureCommitedRight, NULL, &m_commitedRectRight);
	SDL_RenderCopy(renderer, m_textureComposition, NULL, &m_compositionRect);
	//Then the cursor
	if (m_typing && m_cursorTimer < 750) {
		SDL_SetRenderDrawColor(renderer, m_r[Cursor], m_g[Cursor], m_b[Cursor], m_a[Cursor]);
		SDL_RenderFillRect(renderer, &m_cursorRect);
	}

	//Restore previous settings
	SDL_SetRenderTarget(renderer, prevRenderTarget);
	SDL_SetRenderDrawColor(renderer, prevR, prevG, prevB, prevA);

	//std::cout << "Texture has been updated" << std::endl;
}



size_t hui::getUTF8GraphemeIndex(const std::string &s, int graphemeDistance, size_t fromByte) {
	//std::cout << "Moving UTF from byte " << fromByte << " " << graphemeDistance << " graphemes." << std::endl;
	if (fromByte > s.size()) {
		fromByte = s.size() - 1;
		//std::cout << "Cliping from top to range" << std::endl;
	} else if (fromByte < 0) {
		fromByte = 0;
		//std::cout << "Cliping from bottom to range" << std::endl;
	}
	while (s[fromByte] <= (char)0b10111111 && s[fromByte] >= (char)0b10000000 && fromByte > 0) { //Make sure we start on a valid byte
		fromByte--;
		//std::cout << "Getting start byte of current grapheme" << std::endl;
	}

	for (; graphemeDistance > 0 && fromByte < s.size(); graphemeDistance--) { //For moving forward
		fromByte++;
		//std::cout << "Moving forward" << std::endl;
		for (char c = s[fromByte]; c >= (char)0b10000000 && c <= (char)0b10111111 && fromByte < s.size(); fromByte++, c = s[fromByte]); //Skip over non-starting bytes
	}
	for (; graphemeDistance < 0 && fromByte > 0; graphemeDistance++) { //For moving backwards
		fromByte--;
		//std::cout << "Moving forward" << std::endl;
		for (char c = s[fromByte]; c >= (char)0b10000000 && c <= (char)0b10111111 && fromByte > 0; fromByte--, c = s[fromByte]); //Skip over non-starting bytes
	}
	return fromByte;
}