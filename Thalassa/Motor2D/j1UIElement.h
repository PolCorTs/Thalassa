#ifndef __UIELEMENT_H__
#define __UIELEMENT_H__

#include "j1Gui.h"
#include "p2Point.h"
#include "SDL/include/SDL_rect.h"

struct SDL_Texture;
class j1UIElement {

public:
	//Constructor
	j1UIElement(UIELEMENT_TYPES type, int x, int y, SDL_Texture* sprites = nullptr, j1UIElement* parent = nullptr) :
		type(type), position(x, y), initialPosition(x, y), sprites(sprites), parent(parent) {}

	~j1UIElement() {}

	// Blits the element on the screen
	virtual void Draw(float scale, int x = 0, int y = 0, bool use_camera = true) {}

public:
	UIELEMENT_TYPES type;
	bool visible = true;

	iPoint initialPosition;
	iPoint position;

	SDL_Texture* sprites = nullptr;
	j1UIElement* parent = nullptr;
};

#endif // __UIELEMENT_H__
