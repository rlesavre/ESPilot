#ifndef SCREEN_PAGE
#define SCREEN_PAGE


#include "BaseDisplay.h"

class IScreenPage {
public:
	// Should we unify the drawing of line of text ?
	virtual void displayScreenPage(IDisplay *display, int position) = 0;
};

#endif